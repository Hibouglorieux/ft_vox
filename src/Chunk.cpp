#include "Chunk.hpp"
#include <unistd.h>
#include <thread>

#define MAX(x, y) (x)//(x > y ? x : y)
#define TMP_SLEEP_VALUE 0.05 * SEC_TO_MICROSEC

#define NO_TYPE 99


int Chunk::totalChunks = 0;

Chunk::Chunk(int x, int z, Camera* camera)
{
	totalChunks++;
	position = Vec3(x * CHUNK_WIDTH, 0, z * CHUNK_DEPTH);
	//std::cout << "Chunk : " << x << ",0," << z << std::endl;
	// Not necessary ?

	// Should we recompute the chunk's blocs positions
	updateChunk = false;
	// How many solid bloc in chunk
	hardBloc = 0;
	hardBlocVisible = 0;
	init = false;
	threadUseCount = 1;
	/*
	texture = new Texture({
		{"packDefault/GRASS_SIDE.jpg"},
		{"packDefault/GRASS_SIDE.jpg"},
		{"packDefault/GRASS_TOP.jpg"},
		{"packDefault/GRASS_BOTTOM.jpg"},
		{"packDefault/GRASS_SIDE.jpg"},
		{"packDefault/GRASS_SIDE.jpg"}});
		*/
	myNeighbours = {};
	playerCamera = camera;
	glGenBuffers(1, &typeVBO);
	glGenBuffers(1, &positionVBO);

}

Chunk::Chunk(int x, int z, Camera* camera, std::vector<std::pair<Vec2, Chunk*>> neighbours) : Chunk(x, z, camera)
{
	myNeighbours = neighbours;
}

float Chunk::getBlockBiome(int x, int z)
{
	float flatTerrain = VoxelGenerator::Noise2D(position.x + x, position.z + z, 0.4f, 0.00033f, 0.45f, 4, 0, 2, 0.65); // Kind of flat
	flatTerrain = pow(flatTerrain * 0.75, 2);
	//float heightValue = VoxelGenerator::Noise2D(position.x + x, position.z + z, 0.0f, 0.00025f, 2.66f, 2, 1, 2.45, 0.55); // Big hills or mountains ?
	//float heightValue = VoxelGenerator::Noise2D(position.x + x, position.z + z, 0.0f, 0.005f, 0.66f, 1, 1, 2, 1); // Initial form of mountains
	float moutainBiomeValue = VoxelGenerator::Noise2D(position.x + x, position.z + z, 0.0f, 0.000125f, 0.75f, 2, 1, 4, 1); // Nice moutains ? (flat with 2 octaves instead of 1)
	//float heightValue = VoxelGenerator::Noise2D(position.x + x, position.z + z, 0.0f, 0.0025f, 1.2f, 1, 1); // Nice moutains ?

	float heightValue = (flatTerrain * (HEIGHT - 1) * 0.66);

	if (moutainBiomeValue > 0.65) // TODO : Set textures (stone/snow) and add decoration (trees ?)
	{
		//std::cout << moutainBiomeValue << std::endl;
		if (moutainBiomeValue < 0.75) // If value is inferior to 0.70, we smooth the passage from mountain to other biome
		{
			float moutainTerrain = VoxelGenerator::Noise2D(position.x + x, position.z + z, 0.15f, 0.00225f, 1.5f, 1, 0, 3, 0.65); // Nice moutains!
			moutainTerrain = pow(moutainTerrain, 1.66);
			float biomeRange = (0.75 - 0.65);
			float surfaceRange = (moutainTerrain - flatTerrain);
			float interpolationValue = (((moutainBiomeValue - 0.65) * surfaceRange) / biomeRange) + flatTerrain;
			if (interpolationValue > 1)
				interpolationValue = 1;
			heightValue = (interpolationValue * (HEIGHT - 1) * 0.66);
		}
		else	// Biome is fully mountain
		{
			float moutainTerrain = VoxelGenerator::Noise2D(position.x + x, position.z + z, 0.15f, 0.00225f, 1.5f, 1, 0, 3, 1.65); // Nice moutains!
			moutainTerrain = pow(moutainTerrain, 1.66);// * 0.65;
			heightValue = (moutainTerrain * (HEIGHT - 1) * 0.66);
		}
	}
	else if (moutainBiomeValue < 0.55)
	{
		// Create a little mountain biome ? Or canyon or i don't know what
	}	
	
	//float deleteNoise = VoxelGenerator::Noise2D(position.x + x, position.z + z, 0.2, 0.0045, 0.065, 2, 3, 2, 0.5); // Noise used to create hole / entrance to caves in the ground
	//std::cout << deleteNoise << std::endl;

	//std::cout << heightValue << std::endl;
	struct bloc	*bloc = &(blocs[(int)heightValue][z][x]);
	bloc->type = BLOCK_GRASS;
	bloc->visible = 1;
	hardBloc += 1;
	hardBlocVisible++;

	return heightValue;
}

void Chunk::initChunk(void)
{
	struct bloc	*bloc;

	// Get height map for chunk
	blocs = BlocData();// memset equivalent (needed)
	blocs = {NO_TYPE, 0}; // TODO : Check if correct

	int min, max;
	min = CHUNK_HEIGHT + 1;
	max = -1;
	// Set bloc type
	for(unsigned int z = 0; z < CHUNK_DEPTH; z++)
	{
		for(unsigned int x = 0; x < CHUNK_WIDTH; x++)
		{
			bloc = &(blocs[0][z][x]);
			bloc->type = BLOCK_BEDROCK;
			bloc->visible = true; // will be updated after with updateVisibility
			hardBloc++;

			float blockValue = this->getBlockBiome(x, z);
			(void)blockValue;
			if (blockValue < min)
				min = blockValue;
			if (blockValue > max)
				max = blockValue;

			bloc = &(blocs[(int)blockValue][z][x]);
			
			for (int y = (int)blockValue - 1; y > 0; y--)
			{
				bloc = &(blocs[y][z][x]);

				int bloc_type = bloc->type;
				bloc->type = BLOCK_DIRT;
				bloc->visible = 0;
				hardBloc++;
				if (bloc_type != NO_TYPE) // block exists // broken with type because lol
				{
					float xScaled = x / (float)CHUNK_WIDTH + position.x / CHUNK_WIDTH;
					float yScaled = y / (float)CHUNK_HEIGHT;
					float zScaled = z / (float)CHUNK_DEPTH + position.z / CHUNK_DEPTH;
					if (VoxelGenerator::getWorleyValueAt(xScaled, yScaled, zScaled) >= 0.5f) // random value
					{
						bloc->type = NO_TYPE;
						hardBloc--;
						if (bloc->visible)
						{
							hardBlocVisible--;
							bloc->visible = 0;
						}
					}
					else
					{
						bloc->visible = true;
						hardBlocVisible++;
					}
				}
			}
		}
	}

	updateVisibility();
	updateVisibilityByCamera();

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

void	Chunk::worleyCaveTest() // destroying blocks following a 3d worley noise to generate caves
{
	struct bloc	*bloc;
	for(unsigned int y = 3; y < CHUNK_HEIGHT; y++)// random number, can't dig lower than 3, cant dig higher than floor
		for(unsigned int x = 0; x < CHUNK_WIDTH; x++)
			for(unsigned int z = 0; z < CHUNK_DEPTH; z++)
			{
				//if (y >= )// TODO add condition here to make it not being able to make a hole through a mountain (keep it below floor/water level ?). Also needs coherent noise that doesnt start at 0.6f of CHUNK_HEIGHT as minimum result
					//continue;
				bloc = &(blocs[y][z][x]);
				if (bloc->type != NO_TYPE) // block exists // broken with type because lol
				{
					float xScaled = x / (float)CHUNK_WIDTH + position.x / CHUNK_WIDTH;
					float yScaled = y / (float)CHUNK_HEIGHT;
					float zScaled = z / (float)CHUNK_DEPTH + position.z / CHUNK_DEPTH;
					if (VoxelGenerator::getWorleyValueAt(xScaled, yScaled, zScaled) >= 0.8f) // random value
					{			

						//texture = ResourceManager::getBlockTexture(BLOCK_STONE);
						// destroy the block
						bloc->type = NO_TYPE;
						bloc->visible = 0;
						hardBloc--;
						hardBlocVisible--;
					}
				}
			}
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
				if (bloc.type != NO_TYPE && !bloc.visible && neighbourBlocs[y][z][neighbourX].type == NO_TYPE)
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
				if (bloc.type != NO_TYPE && !bloc.visible && neighbourBlocs[y][neighbourZ][x].type == NO_TYPE)
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
	//delete texture;
	glBindBuffer(GL_ARRAY_BUFFER, typeVBO);
	glDeleteBuffers(1, &typeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
	glDeleteBuffers(1, &positionVBO);
	delete heightMap;
	totalChunks--;
}

void Chunk::updateVisibilityByCamera(void)
{
	/*struct bloc	*bloc;
	//hardBlocVisible = 0;

	Vec3 dir = playerCamera->getDirection();
	dir.print();
	playerCamera->getPos().print();
	position.print();*/
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
				if (bloc->type != NO_TYPE)
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
		if (nei->type == NO_TYPE)
		{
			bloc->visible = true;
			//set = true;
			//break;
		}
		//hardNei += nei->type != NO_TYPE ? 1 : 0;
		//invisibleNei += nei->type == NO_TYPE || bloc->visible == false ? 1 : 0;
	}
	//if (hardNei == neighbors.size())
		//bloc->visible = false;
	//else if (invisibleNei == neighbors.size() && !set)
		//bloc->visible = false;
}

bool Chunk::generatePosOffsets(void)
{
	// TODO : Implement way of knowing which bloc should be shown to avoid loading
	//		too much data.
	// Should generate position of each bloc based on the chunk position
	Matrix	mat;
	unsigned int i = 0;
	if (updateChunk)
	{
		GLfloat	*WIP_transform = new GLfloat[hardBlocVisible * 3];//CHUNK_HEIGHT * CHUNK_WIDTH * CHUNK_DEPTH * 3];
		GLint	*WIP_type = new GLint[hardBlocVisible];
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
						WIP_type[i] = blocs[y][z][x].type;
						i += 1;
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
		}
		glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
		glBufferData(GL_ARRAY_BUFFER, hardBlocVisible * 3 * sizeof(float),
			WIP_transform, GL_STATIC_DRAW);
		delete [] WIP_transform;

		glBindBuffer(GL_ARRAY_BUFFER, typeVBO);
		glBufferData(GL_ARRAY_BUFFER, hardBlocVisible * sizeof(GLint),
				WIP_type, GL_STATIC_DRAW);
		delete [] WIP_type;


		updateChunk = false;
		return true;
	}
	return false;
}

void Chunk::draw(Shader* shader)
{
	draw_safe.lock();
	if (hardBloc == 0 || hardBlocVisible == 0 || !init)
	{
		draw_safe.unlock();
		return;
	}
	bool isThereNewData = Chunk::generatePosOffsets();
	(void)isThereNewData; // might be usefull if we use multiple VAO

	//glBindTexture(GL_TEXTURE_CUBE_MAP, texture->getID());	// TODO : Modify in order to use more than one texture at once and those texture should not be loaded here but
															// in ResourceManager.
	RectangularCuboid::drawInstance(shader, positionVBO, typeVBO, hardBlocVisible);
	draw_safe.unlock();
}

Vec2 Chunk::worldCoordToChunk(Vec3 worldPos)
{
	Vec2 pos;
	pos.x = floor(worldPos.x / CHUNK_WIDTH);
	pos.y = floor(worldPos.z / CHUNK_DEPTH);
	return pos;
}
