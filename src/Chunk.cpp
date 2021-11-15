#include "Chunk.hpp"
#include <unistd.h>
#include <thread>

#define MAX(x, y) (x)//(x > y ? x : y)
#define TMP_SLEEP_VALUE 0.05 * SEC_TO_MICROSEC


int Chunk::totalChunks = 0;

Chunk::Chunk(int x, int z)
{
	totalChunks++;
	position = Vec3(x * CHUNK_WIDTH, 0, z * CHUNK_DEPTH);
	//std::cout << "Chunk : " << x << ",0," << z << std::endl;
	// Not necessary ?

	blocsPosition = {};
	// Should we recompute the chunk's blocs positions
	updateChunk = false;
	// How many solid bloc in chunk
	hardBloc = 0;
	hardBlocVisible = 0;
	init = false;
	threadUseCount = 1;
	//texture = new Texture();

	texture = new Texture({
			{"grass/side.jpg"},
			{"grass/side.jpg"},
			{"grass/top.jpg"},
			{"grass/bottom.jpg"},
			{"grass/side.jpg"},
			{"grass/side.jpg"}});
	myNeighbours = {};
}

Chunk::Chunk(int x, int z, std::vector<std::pair<Vec2, Chunk*>> neighbours) : Chunk(x, z)
{
	myNeighbours = neighbours;
}

void Chunk::initChunk(void)
{
	struct bloc	*bloc;
	// Get height map for chunk
	blocs = BlocData();// memset equivalent (needed)
	blocsTests = BlocSearchData();// memset equivalent (needed)
	//heightMap = VoxelGenerator::createMap(position.x / CHUNK_WIDTH, position.z / CHUNK_DEPTH);
	/*heightMap = VoxelGenerator::createMap(position.x / CHUNK_WIDTH,
	  position.z / CHUNK_DEPTH, 7, 2.51525f, 0.7567f);*/
	heightMap = VoxelGenerator::createMap(position.x / CHUNK_WIDTH,
			position.z / CHUNK_DEPTH, 6, 2, 0.5);
	caveMap = VoxelGenerator::createCaveMap(position.x / CHUNK_WIDTH,
			position.z / CHUNK_DEPTH, 5, 3, 0.5);
	// Set bloc type
	for(unsigned int z = 0; z < CHUNK_DEPTH; z++)
	{
		for(unsigned int x = 0; x < CHUNK_WIDTH; x++)
		{
			bloc = &(blocs[0][z][x]);
			bloc->type = 0;
			bloc->visible = 0;// will be updated after with updateVisibility

			float localheight = (*heightMap)[z][x];
			int ty = (int)(localheight * (CHUNK_HEIGHT / 2)
					+ CHUNK_HEIGHT / 3);// % (CHUNK_HEIGHT - 1);
			if (ty >= CHUNK_HEIGHT) // In case of little magic trick is over (or equal to) the max height
			{
				ty = ty % (CHUNK_HEIGHT - 1);
				std::cout << ty << std::endl;
			}

			for (int y = 0; y < ty && y < CHUNK_HEIGHT; y++)
			{
				bloc = &(blocs[y][z][x]);
				bloc->type = 1;
				hardBloc += 1;
			}
		}
	}
	caveTest();
	if (CHUNK_HEIGHT <= 64) // TODO : Make better function, this one crash with HEIGHT > 64
		destroyIlots();

	updateVisibility();
	//if (position.x == 0 && position.z == 0)
		//std::cout << hardBlocVisible << std::endl;;
	init = true;
	updateChunk = true;

	Vec2 myPos = worldCoordToChunk(getPos());
	std::vector<std::thread> threads;
	for (auto it : myNeighbours)
	{
		threadUseCount++;
		Vec2 neighbourPos = it.first;
		auto callBack = [this, neighbourPos]
			(const BlocData& neighbourBlocs)
			{this->updateVisibilityWithNeighbour(neighbourPos, neighbourBlocs, nullptr);
			};
		auto threadFunc = [myPos, callBack](const BlocData& bd, Chunk* neighbour){neighbour->updateVisibilityWithNeighbour(myPos, bd, callBack);};
		threads.push_back(std::thread(threadFunc, blocs, it.second));
		//it.second->updateVisibilityWithNeighbour(myPos, blocs, callBack);
	}
	for (std::thread& worker : threads)
	{
		worker.join();
	}
	threadUseCount--;
}

void	Chunk::caveTest() // destroying blocks following a 3d map to generate caves
{
	struct bloc	*bloc;
	for(unsigned int y = 3; y < CHUNK_HEIGHT; y++)// random number, can dig lower than 3
		for(unsigned int x = 0; x < CHUNK_WIDTH; x++)
			for(unsigned int z = 0; z < CHUNK_DEPTH; z++)
			{
				if (y >= 64.0f)
					continue;
				bloc = &(blocs[y][z][x]);
				if (bloc->type != 0) // block exists
					if ((*caveMap)[y][z][x] >= 0.4f) // random value
					{
						// destroy the block
						bloc->type = 0;
						hardBloc--;
					}
			}
}

void Chunk::destroyIlots()
{
	// 	This function goal is to delete floating block at initilization, not after player removed some and created a floating block.
	// 	So this function should only be used at init of chunk
	//
	//	To delete floating block(s), the function must be able to check if the bloc is connected to the ground.
	//	To check if a block is connected to the ground we need to be able to find its neighbors, we will try the worst way first which is
	//	to look for each neighbor recursively till we get a block at y=0.
	// 
	//	If that function return False, then we delete the block group, else it stays

	//	1-st Step : Find blocks that do not have solid neighbors
	struct bloc					*block;
	std::vector<struct bloc*>	blockGroup = {};
	//BlocSearchData				visitedGroup = BlocSearchData();
	Vec3						pos;
	bool						floating = false;
	blocsTests = BlocSearchData();
	for(unsigned int y = CHUNK_HEIGHT; y > 0; y--)
		for(unsigned int x = 0; x < CHUNK_WIDTH; x++)
			for(unsigned int z = 0; z < CHUNK_DEPTH; z++)
			{
				if (blocsTests[y - 1][z][x] == 1)
					continue;
				block = &(blocs[y - 1][z][x]);
				if (block->type == 0)
					continue;
				else if (block->type != 0)
				{
					blockGroup.clear();
					pos = Vec3(x, y - 1, z);
					floating = !destroyIlotsSearchAndDestroy(block, pos, &blockGroup);
					if (floating) // Floating groups detected, will now replace each block in blockGroup by air
					{
						for (auto it = blockGroup.begin(); it != blockGroup.end(); it++)
						{
							(*it)->type = 0;
						}
					}
					blockGroup.clear();
				}
			}
	blockGroup.clear();
}

bool Chunk::destroyIlotsSearchAndDestroy(struct bloc *block, Vec3 pos, std::vector<struct bloc*> *blockGroup) // Return true if block isn't connected to ground, false if connected to a block at y=0
{
	bool	anchored = false;
	bool	anchored_tmp = false;
	
	int x, y, z;
	x = (int)(pos.x);
	y = (int)(pos.y);
	z = (int)(pos.z);

	(blocsTests[y][z][x]) = 1;
	blockGroup->push_back(block);

	if (pos.y == 0 && block->type != 0)	// Block is the floor, group is valid, we only keep visited block in memory, clear blockGroup
	{
		blockGroup->clear();
		return true;
	}

	//	First we retrieve the neighbors, avoid the one we already visited
	std::vector<struct bloc*>	neighbors = {};
	std::vector<Vec3>			neighborsPos = {};
	if (y > 0 && (blocs[y - 1][z][x]).type != 0)
	{
		neighbors.push_back(&(blocs[y - 1][z][x]));
		neighborsPos.push_back(Vec3(x, y - 1, z));
	}
	if (y < CHUNK_HEIGHT - 1 && (blocs[y + 1][z][x]).type != 0)
	{
		neighbors.push_back(&(blocs[y + 1][z][x]));
		neighborsPos.push_back(Vec3(x, y + 1, z));
	}
	if (x > 0 && (blocs[y][z][x - 1]).type != 0)
	{
		neighbors.push_back(&(blocs[y][z][x - 1]));
		neighborsPos.push_back(Vec3(x - 1, y, z));
	}
	if (x < CHUNK_WIDTH - 1 && (blocs[y][z][x + 1]).type != 0)
	{
		neighbors.push_back(&(blocs[y][z][x + 1]));
		neighborsPos.push_back(Vec3(x + 1, y, z));
	}
	if (z > 0 && (blocs[y][z - 1][x]).type != 0)
	{
		neighbors.push_back(&(blocs[y][z - 1][x]));
		neighborsPos.push_back(Vec3(x, y, z - 1));
	}
	if (z < CHUNK_DEPTH - 1 && (blocs[y][z + 1][x]).type != 0)
	{
		neighbors.push_back(&(blocs[y][z + 1][x]));
		neighborsPos.push_back(Vec3(x, y, z + 1));
	}

	// Second, we iterate through the neighbors and seek their neighbor is they haven't already been visited
	for(int nei = 0; (unsigned int)nei < neighbors.size(); ++nei) {
		Vec3 npos = neighborsPos[nei];
		if (blocsTests[(int)npos.y][(int)npos.z][(int)npos.x] == 1)
			continue;
		anchored_tmp = destroyIlotsSearchAndDestroy(neighbors[nei], neighborsPos[nei], blockGroup);
		if (anchored_tmp && !anchored) 		// Meaning a block rattached to the current block is the floor so the group is valid
		{
			anchored = true;	// We let the function continue to mark all connected blocks
		}
	}
	if (anchored)
	{
		if (blockGroup->size() != 0)
			blockGroup->clear();
		return true;
	}
	return false; // Meaning no block rattached to the current block is touching the floor, group is invalid and must be deleted (replaced by air)
}

void Chunk::updateVisibilityWithNeighbour(Vec2 NeighbourPos, const BlocData& neighbourBlocs, std::function<void(const BlocData&)> callBack)
{
	Vec2 relativePos = NeighbourPos - worldCoordToChunk(getPos());

	while (!init) // this happens when a neighbours has finished initializing but the one being called to update hasnt finished yet
	{
		usleep(TMP_SLEEP_VALUE);
	}
	draw_safe.lock();
	if (relativePos.x != 0)
	{
		int x = relativePos.x == -1 ? 0 : CHUNK_WIDTH - 1;
		int neighbourX = x == 0 ? CHUNK_WIDTH - 1 : 0;
		for(int y = 0; y < CHUNK_HEIGHT; y++)
			for (int z = 0; z < CHUNK_DEPTH; z++)
			{
				struct bloc& bloc = blocs[y][z][x];
				if (bloc.type != 0 && !bloc.visible && neighbourBlocs[y][z][neighbourX].type == 0)
				{
					hardBlocVisible++;
					bloc.visible = true;
					updateChunk = true;
				}
			}
	}
	else  // relativePos.y != 0
	{
		int z = relativePos.y == -1 ? 0 : CHUNK_DEPTH - 1;
		int neighbourZ = z == 0 ? CHUNK_DEPTH - 1 : 0;
		for(int y = 0; y < CHUNK_HEIGHT; y++)
			for (int x = 0; x < CHUNK_WIDTH; x++)
			{
				struct bloc& bloc = blocs[y][z][x];
				if (bloc.type != 0 && !bloc.visible && neighbourBlocs[y][neighbourZ][x].type == 0)
				{
					hardBlocVisible++;
					bloc.visible = true;
					updateChunk = true;
				}
			}
	}
	draw_safe.unlock();
	if (callBack)
		callBack(blocs);
	threadUseCount--;
}

Chunk::~Chunk(void)
{
	delete [] blocsPosition;
	delete texture;
	delete heightMap;
	totalChunks--;
}

void Chunk::updateVisibility(void)
{
	struct bloc	*bloc;
	hardBlocVisible = 0;
	for(int y = CHUNK_HEIGHT; y > 0; y--)
	{
		for(int z = CHUNK_DEPTH; z > 0; z--)
		{
			for(int x = CHUNK_WIDTH; x > 0; x--)
			{
				bloc = &(blocs[y - 1][z - 1][x - 1]);
				if (bloc->type != 0)
				{
					setVisibilityByNeighbors(x - 1, y - 1, z - 1);
					if (bloc->visible == true)
						hardBlocVisible += 1;
				}
			}
		}
	}
}


void Chunk::setVisibilityByNeighbors(int x, int y, int z) // Activates visibility if one neighbour is transparent
{
	// TODO : Check coordinate to avoid overflow
	struct bloc			*bloc = &(blocs[y][z][x]);
	std::vector<struct bloc*>	neighbors = {};
	//long unsigned int			hardNei = 0;
	//long unsigned int			invisibleNei = 0;
	//bool						set = false;

	if (y > 0)
		neighbors.push_back(&(blocs[y - 1][z][x]));
	if (y < CHUNK_HEIGHT - 1)
		neighbors.push_back(&(blocs[y + 1][z][x]));
	if (x > 0)
		neighbors.push_back(&(blocs[y][z][x - 1]));
	if (x < CHUNK_WIDTH - 1)
		neighbors.push_back(&(blocs[y][z][x + 1]));
	if (z > 0)
		neighbors.push_back(&(blocs[y][z - 1][x]));
	if (z < CHUNK_DEPTH - 1)
		neighbors.push_back(&(blocs[y][z + 1][x]));
	for (struct bloc *nei: neighbors)
	{
		if (nei->type == 0)
		{
			bloc->visible = true;
			//set = true;
			//break;
		}
		//hardNei += nei->type != 0 ? 1 : 0;
		//invisibleNei += nei->type == 0 || bloc->visible == false ? 1 : 0;
	}
	//if (hardNei == neighbors.size())
		//bloc->visible = false;
	//else if (invisibleNei == neighbors.size() && !set)
		//bloc->visible = false;
}


GLfloat *Chunk::generatePosOffsets(void)
{
	// TODO : Implement way of knowing which bloc should be shown to avoid loading
	//		too much data.
	// Should generate position of each bloc based on the chunk position
	Matrix	mat;
	unsigned int i = 0;
	if (updateChunk)
	{
		GLfloat	*WIP_transform = new GLfloat[hardBlocVisible * 3];//CHUNK_HEIGHT * CHUNK_WIDTH * CHUNK_DEPTH * 3];
		for(unsigned int y = 0; y < CHUNK_HEIGHT; y++)	// Too big of a loop
		{
			for(unsigned int z = 0; z < CHUNK_DEPTH; z++)
			{
				for(unsigned int x = 0; x < CHUNK_WIDTH; x++)
				{
					unsigned int indexX = 0;
					unsigned int indexY = 0;
					unsigned int indexZ = 0;
					if (blocs[y][z][x].visible == 1)
					{
						indexX = i * 3;
						indexY = i * 3 + 1;
						indexZ = i * 3 + 2;
						i += 1;
					}
					else
						continue;
					//std::cout << indexX << " ; " << indexY << " ; " << indexZ << "\n\n";
					WIP_transform[indexX] = position.x + x;
					WIP_transform[indexY] = position.y + y;
					WIP_transform[indexZ] = position.z + z;
					/*std::cout << WIP_transform[indexX] << ";";
					  std::cout << WIP_transform[indexY] << ";";
					  std::cout << WIP_transform[indexZ] << "\n";*/
				}
			}
		}
		updateChunk = false;
		delete [] blocsPosition;
		blocsPosition = WIP_transform;
	}
	return blocsPosition;
}

void Chunk::draw(Shader* shader)
{
	draw_safe.lock();
	if (hardBloc == 0 || hardBlocVisible == 0 || !init)
	{
		draw_safe.unlock();
		return;
	}
	// Should draw instantiated bloc of same type once for each type.
	GLfloat	*positionOffset = Chunk::generatePosOffsets();

	glBindTexture(GL_TEXTURE_CUBE_MAP, texture->getID());
	RectangularCuboid::drawInstance(shader, texture,
			positionOffset, hardBlocVisible);
	draw_safe.unlock();
}

Vec2 Chunk::worldCoordToChunk(Vec3 worldPos)
{
	Vec2 pos;
	pos.x = floor(worldPos.x / CHUNK_WIDTH);
	pos.y = floor(worldPos.z / CHUNK_DEPTH);
	return pos;
}
