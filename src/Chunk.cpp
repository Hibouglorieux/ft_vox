#include "Chunk.hpp"
#include <unistd.h>
#include <thread>

#define MAX(x, y) (x)//(x > y ? x : y)
#define TEST 0.05 * SEC_TO_MICROSEC


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
	heightMap = VoxelGenerator::createMap(position.x / CHUNK_WIDTH, position.z / CHUNK_DEPTH);

	// Set bloc type
	for(unsigned int y = 0; y < CHUNK_HEIGHT; y++)	// Too big of a loop
	{
		for(unsigned int x = 0; x < CHUNK_WIDTH; x++)
		{
			for(unsigned int z = 0; z < CHUNK_DEPTH; z++)
			{
				bloc = &(blocs[y][z][x]);
				if (MAX((*heightMap)[z][x], 0.5) * CHUNK_HEIGHT - 1 < y)// TODO tmp to remove -1 when CHUNK has to check above/neighbours
				{
					bloc->type = 0;
					bloc->visible = 0;
				}
				else
				{
					bloc->type = 1;
					bloc->visible = 1;
					hardBloc += 1;
				}
			}
		}
	}
	updateVisibility();
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

void Chunk::updateVisibilityWithNeighbour(Vec2 NeighbourPos, const BlocData& neighbourBlocs, std::function<void(const BlocData&)> callBack)
{
	Vec2 relativePos = NeighbourPos - worldCoordToChunk(getPos());

	while (!init)
		usleep(TEST);
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
				if (bloc->type == 0)
					bloc->visible = 0;
				else
				{
					setVisibilityByNeighbors(x - 1, y - 1, z - 1);
					if (bloc->visible == true)
						hardBlocVisible += 1;
				}
			}
		}
	}
}

void Chunk::setVisibilityByNeighbors(int x, int y, int z)
{
	// TODO : Check coordinate to avoid overflow
	struct bloc			*bloc = &(blocs[y][z][x]);
	std::vector<struct bloc*>	neighbors = {};
	long unsigned int			hardNei = 0;
	long unsigned int			invisibleNei = 0;
	bool						set = false;

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
			set = true;
			break;
		}
		hardNei += nei->type != 0 ? 1 : 0;
		invisibleNei += nei->type == 0 || bloc->visible == false ? 1 : 0;
	}
	if (hardNei == neighbors.size())
		bloc->visible = false;
	else if (invisibleNei == neighbors.size() && !set)
		bloc->visible = false;
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
