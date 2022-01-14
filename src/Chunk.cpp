#include "Chunk.hpp"
#include <unistd.h>
#include <thread>

#define MAX(x, y) (x) //(x > y ? x : y)
#define TMP_SLEEP_VALUE 0.05 * SEC_TO_MICROSEC

//#define NO_TYPE 99

int Chunk::totalChunks = 0;

Chunk::Chunk(int x, int z, Camera *camera)
{
	totalChunks++;
	position = Vec3(x * CHUNK_WIDTH, 0, z * CHUNK_DEPTH);
	//position.print();
	//std::cout << "Chunk : " << x << ",0," << z << std::endl;
	// Not necessary ?

	// Should we recompute the chunk's blocs positions
	updateChunk = false;

	// How many solid bloc in chunk
	hardBloc = 0;
	hardBlocVisible = 0;
	init = false;
	threadUseCount = 1;

	spaceCount = 0;
	//spaceBorder = { 0 };
	spaceESize = { 0 };
	
	myNeighbours = {};
	playerCamera = camera;
	glGenBuffers(1, &typeVBO);
	glGenBuffers(1, &positionVBO);
	glGenBuffers(1, &facesVBO);
}

Chunk::Chunk(int x, int z, Camera *camera, std::vector<std::pair<Vec2, Chunk *>> neighbours) : Chunk(x, z, camera)
{
	(void)neighbours;
	//myNeighbours = neighbours;
}

/*
float Chunk::getBlockBiome(int x, int z, bool setBlocInChunk, bool superFlat)
{
	// First step :
	//	Compute flat floor at height 63 (Chunk being 256 bloc tall)
	float heightValue = 63;
	if (!superFlat)
	{
		// Second step :
		//	Generate Temperature & Humidity value of the block
		//float temperature	= VoxelGenerator::Noise2D(position.x + x, position.z + z, 0.0f, 0.000125f, 0.75f, 2, 1, 4, 1) * MAX_TEMPERATURE; 		// TODO : Determine correct value
		//float humidity		= VoxelGenerator::Noise2D(position.x + x, position.z + z, 0.0f, 0.000125f, 0.75f, 2, 1, 2, 0.5) * 100.0;				// TODO : Get different value than temperature
		//int biome = BasicBiome;	// Set biome according to biome table

		float biomeSelector 		= VoxelGenerator::Noise2D(position.x + x, position.z + z, 0, 0.00025f, 0.85f, 2, 0, 2.0f, 0.5f);

		float flatTerrain 			= VoxelGenerator::Noise2D(position.x + x, position.z + z, 0.0f, 0.000133f, 0.25f, 4, 0, 2, 0.65);
		float hillTerrain			= VoxelGenerator::Noise2D(position.x + x, position.z + z, 0.0f, 0.000125f, 0.55f, 2, 1, 4, 1);
		float mountainTerrain		= VoxelGenerator::Noise2D(position.x + x, position.z + z, 0.0f, 0.00525f, 0.75f, 1, 0, 3, 0.65);
		float desertTerrain			= VoxelGenerator::Noise2D(position.x + x, position.z + z, 0.0f, 0.000525f, 0.15f, 2, 1, 4, 1);

		if (biomeSelector < 0.3)		// Basic
			heightValue = 64;
		else if (biomeSelector < 0.5)	// Hills
			heightValue = 70;
		else if (biomeSelector < 0.7)	// Moutain
			heightValue = 80;
		else
			heightValue = mountainTerrain * (HEIGHT - 1);
		float value = biomeSelector;
		//printf("%f\n", value);

		if (value < 0)
			value = 0;
		else if (value > 1)
			value = 1;

		heightValue = value * (HEIGHT - 1);

// Third step :
//	Determine the bloc style, type & height
switch (biome)
		{
			case BasicBiome:
				// Set biome matching terrain
				break;
			case ForestBiome:
				// Set biome matching terrain
				break;
			case DesertBiome:
				// Set biome matching terrain
				break;
			case MountainBiome:
				// Set biome matching terrain
				break;
			
			default:
				break;
		}

		// Fourth step :
		//	Check if there is a river
	}

	// Final step :
	//	If setBlocInChunk is true, set the bloc using what has been determined at the previous steps
	if (!setBlocInChunk)
		return heightValue;
	struct bloc	*bloc = &(blocs[(int)heightValue][z][x]);
	bloc->type = BLOCK_GRASS;
	if (heightValue >= 70)
		bloc->type = BLOCK_SAND;
	if (heightValue >= 80)
		bloc->type = BLOCK_STONE;
	bloc->visible = 1;
	hardBloc += 1;
	hardBlocVisible++;

	return heightValue;
}*/

static inline float lerp(float a, float b, float f)
{
	return a + f * (b - a);
}

float Chunk::getBlockBiome(int x, int z, bool setBlocInChunk)
{
	float flatTerrain = VoxelGenerator::Noise2D(position.x + x, position.z + z, 0.2f, 0.00033f, 0.25f, 4, 0, 2, 0.65); // Kind of flat
	flatTerrain = pow(flatTerrain * 0.75, 2);
	float terrainBiomeValue = VoxelGenerator::Noise2D(position.x + x, position.z + z, 0.0f, 0.000125f, 0.75f, 2, 1, 4, 1);

	int blocType = BLOCK_GRASS;

	float heightValue = (flatTerrain * (HEIGHT - 1) * 0.66);

	if (terrainBiomeValue > 0.45)
	{
		float moutainTerrain = VoxelGenerator::Noise2D(position.x + x, position.z + z, 0.15f, 0.00225f, 2.5f, 1, 0, 2.13f, 0.65);
		moutainTerrain = pow(moutainTerrain, 1.66);

		float biomeRange = (1.0 - 0.45);
		float surfaceRange = (moutainTerrain - flatTerrain);
		float interpolationValue = (((terrainBiomeValue - 0.45) * surfaceRange) / biomeRange) + flatTerrain;

		if (interpolationValue > 1)
			interpolationValue = 1;
		if (interpolationValue < 0.0)
			interpolationValue = 0;
		heightValue = (interpolationValue * (HEIGHT - 1) * 0.66);
		if (heightValue > 120)
			blocType = BLOCK_SNOW;
		else if (heightValue > 80 && moutainTerrain > 0.88)
			blocType = BLOCK_STONE;
	}
	else if (terrainBiomeValue < 0.45)
	{
		// Forest ?
	}

	if (!setBlocInChunk)
		return heightValue;

	struct bloc *bloc = &(blocs[(int)heightValue][z][x]);

	bloc->type = blocType;

	bloc->visible = 1;
	hardBloc += 1;
	hardBlocVisible++;

	return heightValue;
}

void Chunk::initChunk(void)
{
	struct bloc *bloc;

	// Get height map for chunk
	blocs = BlocData();		 // memset equivalent (needed)
	blocs = {NO_TYPE, 0, -1, 0, 0}; // TODO : Check if correct

	int min, max;
	min = CHUNK_HEIGHT + 1;
	max = -1;
	// Set bloc type
	for (unsigned int z = 0; z < CHUNK_DEPTH; z++)
	{
		for (unsigned int x = 0; x < CHUNK_WIDTH; x++)
		{
			for (unsigned int y = 0; y < CHUNK_HEIGHT; y++)
			{
				bloc = &(blocs[y][z][x]);
				if (y == 0)
				{
					bloc->type = BLOCK_BEDROCK;
					hardBloc++;
				}
				else
					bloc->type = NO_TYPE;
				bloc->visible = false;
				bloc->spaceId = -1;
				bloc->visited = false;
			}

			float blockValue = this->getBlockBiome(x, z);

			if (blockValue < min)
				min = blockValue;
			if (blockValue > max)
				max = blockValue;

			for (int y = (int)blockValue - 1; y > 0; y--)
			{
				bloc = &(blocs[y][z][x]);
				int bloc_type = bloc->type;

				if ((&(blocs[(int)blockValue][z][x]))->type == BLOCK_GRASS || (&(blocs[(int)blockValue][z][x]))->type == NO_TYPE)
					bloc->type = BLOCK_DIRT;
				else
					bloc->type = BLOCK_STONE;
				bloc->visible = 0;
				hardBloc++;

				/*if (bloc_type == NO_TYPE && y < blockValue - 3 && y > 0) // block exists // broken with type because lol
				{
					float xScaled = (x + position.x) * (32.0 / 1024.0); //(x / (float)CHUNK_WIDTH + position.x / CHUNK_WIDTH);// * ((float)WORLEY_SIZE / 512.0f);
					float yScaled = y * 0.5;//  * (32.0 / 1024.0);
					float zScaled = (z + position.z)  * (32.0 / 1024.0); //(z / (float)CHUNK_DEPTH + position.z / CHUNK_DEPTH);// * ((float)WORLEY_SIZE / 512.0f);
					if (VoxelGenerator::getWorleyValueAt(xScaled, yScaled, zScaled) >= 0.83f) // random value
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
				}*/
			}
		}
	}
	//std::cout << max << std::endl;
	if (max < CHUNK_HEIGHT)
		max = max + 30;
	boundingVolume = AABB(position, Vec3(position.x + CHUNK_WIDTH, max, position.z + CHUNK_DEPTH));
	//boundingVolume = AABB(Vec3(0, 0, 0), Vec3(CHUNK_WIDTH, max, CHUNK_DEPTH));
	//std::cout << "Is on frustum : " << boundingVolume.isOnFrustum(playerCamera->getFrustum()) << std::endl;

	updateVisibility();
	updateVisibilitySpace();
	//updateVisibilityByCamera();
	//generateConnectedSpaces();

	init = true;
	updateChunk = true;

	Vec2 myPos = worldCoordToChunk(getPos());
	threadUseCount = 0;
}

Chunk::~Chunk(void)
{
	//delete texture;
	glBindBuffer(GL_ARRAY_BUFFER, typeVBO);
	glDeleteBuffers(1, &typeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
	glDeleteBuffers(1, &positionVBO);
	//delete heightMap;
	totalChunks--;
}

void Chunk::updateVisibilityWithNeighbour(Vec2 NeighbourPos, const BlocData &neighbourBlocs, std::function<void(const BlocData &)> callBack)
{
	Vec2 relativePos = NeighbourPos - worldCoordToChunk(getPos());

	while (!init) // this happens when a neighbours has finished initializing but the one being called to update hasnt finished yet
	{
		printf("I'm not ready yet !");
		usleep(TMP_SLEEP_VALUE);
	}
	draw_safe.lock();
	if (relativePos.x != 0)
	{
		int x = relativePos.x == -1 ? 0 : CHUNK_WIDTH - 1;
		int neighbourX = x == 0 ? CHUNK_WIDTH - 1 : 0;
		for (int y = 0; y < CHUNK_HEIGHT; y++)
			for (int z = 0; z < CHUNK_DEPTH; z++)
			{
				struct bloc &bloc = blocs[y][z][x];
				if (bloc.type != NO_TYPE && !bloc.visible && neighbourBlocs[y][z][neighbourX].type == NO_TYPE)
				{
					hardBlocVisible++;
					bloc.visible = true;
					updateChunk = true;
				}
			}
	}
	else // relativePos.y != 0
	{
		int z = relativePos.y == -1 ? 0 : CHUNK_DEPTH - 1;
		int neighbourZ = z == 0 ? CHUNK_DEPTH - 1 : 0;
		for (int y = 0; y < CHUNK_HEIGHT; y++)
			for (int x = 0; x < CHUNK_WIDTH; x++)
			{
				struct bloc &bloc = blocs[y][z][x];
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

void Chunk::updateVisibilityByCamera(bool freeze)
{
	// Raycasting -> Bad idea
	// This functions does not seems like a good idea.
	// It is not working as intended and destroying perf for the moment
	struct bloc *bloc;
	if (freeze || !init)
		return;

	Frustum playerFrustum = playerCamera->getFrustum();
	Vec3 playerSight = playerCamera->getDirection();
	//float sightLen = playerSight.getLength();
	Vec3 playerPos = playerCamera->getPos();
	float half_fov = 1.0 / 180.0 * (FOV * 1.2);

	for (int y = CHUNK_HEIGHT; y > 0; y--)
	{
		for (int z = CHUNK_DEPTH; z > 0; z--)
		{
			for (int x = CHUNK_WIDTH; x > 0; x--)
			{
				bloc = &(blocs[y - 1][z - 1][x - 1]);

				// Compute line of sight from block to player.
				if (bloc->type != NO_TYPE)
				{
					// Block position
					Vec3 blockPosition = Vec3(position + Vec3(x - 1, y - 1, z - 1));
					// Direction vector from block to camera
					Vec3 dirToCamera = Vec3(playerPos - blockPosition).getNormalized();
					//float dirToLength = dirToCamera.getLength();

					// Angle between the camera direction and the direction
					// vector of the block to camera
					float value = dirToCamera.dot(playerSight);
					//float cos = value / (dirToLength * sightLen);

					/*Vec3 cross = dirToCamera.cross(playerSight);
					  float sin = cross.getLength() / (dirToLength * sightLen);

					  float angle = std::acos(cos) * 180.0 / M_PI;*/
					float angle = value;
					//if (sin < 0)
					//	angle = -angle;
					//printf("Sight value : %f\n", angle);
					//printf("%f\n", half_fov);
					if (angle <= -1 + half_fov && angle >= -1)
					{
						if (bloc->visible == false)
						{
							//printf("Value : %f >= %f >= %f\n", FOV, value, -FOV);
							//printf("Showing block\n");
							bloc->visible = true;
							hardBlocVisible++;
						}
					}
					else
					{
						if (bloc->visible)
						{
							//printf("Value : %f >= %f >= %f\n", FOV, value, -FOV);
							//printf("Hiding block\n");
							hardBlocVisible--;
						}
						bloc->visible = false;
					}
				}
			}
		}
	}
	//printf("\n");
	//printf("Block visible : %i\n", hardBlocVisible);

	updateChunk = true;
	//if (hardBlocVisible > 0)
	//	Chunk::generatePosOffsets();
	//draw_safe.unlock();
}

void Chunk::updateVisibility(void)
{
	struct bloc *bloc;
	hardBlocVisible = 0;
	for (int y = CHUNK_HEIGHT; y > 0; y--)
	{
		for (int z = CHUNK_DEPTH; z > 0; z--)
		{
			for (int x = CHUNK_WIDTH; x > 0; x--)
			{
				bloc = &(blocs[y - 1][z - 1][x - 1]);
				if (bloc->type != NO_TYPE)
				{
					GLuint visibleFaces = setVisibilityByNeighbors(x - 1, y - 1, z - 1);
					if (bloc->visible == true)
					{
						hardBlocVisible += 1;
						//facesToRender.push_back(visibleFaces);
					}
				}
				/*else
				{
					bloc->spaceId = spaceCount;
					spaceCount++;
				}*/
			}
		}
	}
}

/*
 * Function should only add bloc which are at the border of the chunk
 */
void Chunk::updateVisibilityBorder(int x, int y, int z, int xHeight, int zHeight, bool master)
{
	struct bloc *bloc = &(blocs[y][z][x]);
	std::vector<std::pair<float, GLuint>> border_neighbors = {};
	int maxDiff = 0;

	// Get the neigbhors height once
	if (xHeight == -1)
	{
		if (x == 0)
			xHeight = (int)(this->getBlockBiome(x - 1, z, false));
		else if (x == CHUNK_WIDTH - 1)
			xHeight = (int)(this->getBlockBiome(x + 1, z, false));
	}
	if (zHeight == -1)
	{
		if (z == 0)
			zHeight = (int)(this->getBlockBiome(x, z - 1, false));
		else if (z == CHUNK_DEPTH - 1)
			zHeight = (int)(this->getBlockBiome(x, z + 1, false));
	}

	// If both height are superior to current y, no block under us need to be displayed
	if (xHeight >= y && zHeight >= y)
		return;
	
	GLuint faces = 0;
	if (xHeight > -1 && xHeight < y)
		faces |= x == 0 ? LEFT_NEIGHBOUR : RIGHT_NEIGHBOUR;
	if (zHeight > -1 && zHeight < y)
		faces |= z == 0 ? BACK_NEIGHBOUR : FRONT_NEIGHBOUR;

	if (faces == 0)
		return;

	facesToRender.push_back(faces);
	spaceBorder[spaceCount].push_back(Vec3(x, y, z));

	if (master)
	{
		if (xHeight > -1 && zHeight > -1)
			maxDiff = xHeight > zHeight ? zHeight : xHeight;
		if (xHeight > -1 || zHeight > -1)
			maxDiff = xHeight > -1 ? xHeight : zHeight;
		for (int yDiff = 1; y - yDiff > maxDiff; yDiff++)
			updateVisibilityBorder(x, y - yDiff, z, xHeight, zHeight);
	}
}

void Chunk::updateVisibilitySpaceAux(int ox, int oy, int oz)
{
	std::vector<Vec3> stack;
	Vec3	cur;
	float	x, y, z;
	int		spaceBlocCount;

	// Clear the stack and the visited stack
	stack.clear();
	// Set the starting point
	cur = Vec3(ox, oy, oz);
	stack.push_back(cur);
	spaceBlocCount = 0;

	//printf("%i\n", spaceBlocCount);
	// While the stack is not empty, look for neighbors
	while (!stack.empty())
	{
		// Get the current position
		cur = stack.back();
		x = cur.x;
		y = cur.y;
		z = cur.z;
		stack.pop_back();

		// Check if we're looking at already visited positions
		if (blocs[cur.y][cur.z][cur.x].visited)
			continue;
		(&(blocs[cur.y][cur.z][cur.x]))->visited = true;
		// Check if the bloc is of NO_TYPE type
		// If not, add it to the current border space
		if (blocs[cur.y][cur.z][cur.x].type != NO_TYPE)
		{
			if (spaceCount == 0)
				facesToRender.push_back(blocs[cur.y][cur.z][cur.x].faces);
			spaceBorder[spaceCount].push_back(cur);
			if (cur.y > 0 && (cur.x == 0 || cur.x == CHUNK_WIDTH - 1 || cur.z == 0 || cur.z == CHUNK_DEPTH - 1))
				updateVisibilityBorder(cur.x, cur.y - 1, cur.z, -1, -1, true);
			continue;
		}

		if (x > 0)
			stack.push_back(Vec3(x - 1, y, z));
		if (x < CHUNK_WIDTH - 1)
			stack.push_back(Vec3(x + 1, y, z));
		if (z > 0)
			stack.push_back(Vec3(x, y, z - 1));
		if (z < CHUNK_DEPTH - 1)
			stack.push_back(Vec3(x, y, z + 1));
		if (y > 0)
			stack.push_back(Vec3(x, y - 1, z));
		if (y < CHUNK_HEIGHT - 1)
			stack.push_back(Vec3(x, y + 1, z));

		(&(blocs[y][z][x]))->spaceId = spaceCount;
		spaceBlocCount++;
	}
	//printf("%i\n", spaceBlocCount);
	spaceESize[spaceCount] = spaceBlocCount;
}

void Chunk::updateVisibilitySpace(void)
{
	struct bloc *bloc;
	bool inSpace = false;
	
	for (int y = CHUNK_HEIGHT; y > 0; y--)
	{
		for (int z = CHUNK_DEPTH; z > 0; z--)
		{
			for (int x = CHUNK_WIDTH; x > 0; x--)
			{
				bloc = &(blocs[y - 1][z - 1][x - 1]);
				if (inSpace && bloc->type != NO_TYPE) // Add block to space border (which will be rendered if player is in this space)
				{
					//
				}
				if (bloc->type == NO_TYPE)
				{
					inSpace = true;
					if (bloc->spaceId != spaceCount && bloc->spaceId != -1) // Compare size of both space and keep the biggest one (merge ecount + borders (keep unique))
					{
						bool fbigger = spaceESize[spaceCount] > spaceESize[bloc->spaceId];
						//
					}
					else if (bloc->spaceId == -1) // Set current spaceId and propagate it to neighbors then increase spaceId
					{
						updateVisibilitySpaceAux(x - 1, y - 1, z - 1);
						//printf("Space border count : %li\n", spaceBorder[spaceCount].size());
						spaceCount++;
					}

					if (x == 0 || x == CHUNK_WIDTH || z == 0 || z == CHUNK_DEPTH)	// TODO : updateVisibilitySpaceAux should do it
					{
						// The spaceId of the current bloc is connected to the next chunk
						// x == 0 right chunk else left chunk
						// z == 0 backward chunk else forward chunk
					}
				}
			}
		}
	}
	//printf("Number of space : %i\n", spaceCount);
	//printf("Space border count : %5li | Visible blocs : %5i\n", spaceBorder[0].size(), hardBlocVisible);
}

GLuint Chunk::setVisibilityByNeighbors(int x, int y, int z) // Activates visibility if one neighbour is transparent
{
	struct bloc *bloc = &(blocs[y][z][x]);
	std::vector<std::pair<struct bloc *, GLuint>> neighbors = {};
	std::vector<std::pair<float, GLuint>> border_neighbors = {};
	GLuint visibleFaces = 0;

	// Check with neighbors case using noise
	if (x == 0)
		border_neighbors.push_back({this->getBlockBiome(x - 1, z, false), LEFT_NEIGHBOUR});
	else if (x == CHUNK_WIDTH - 1)
		border_neighbors.push_back({this->getBlockBiome(x + 1, z, false), RIGHT_NEIGHBOUR});
	if (z == 0)
		border_neighbors.push_back({this->getBlockBiome(x, z - 1, false), BACK_NEIGHBOUR});
	else if (z == CHUNK_DEPTH - 1)
		border_neighbors.push_back({this->getBlockBiome(x, z + 1, false), FRONT_NEIGHBOUR});

	if (y > 0)
		neighbors.push_back({&(blocs[y - 1][z][x]), BOTTOM_NEIGHBOUR});
	if (y < CHUNK_HEIGHT - 1)
		neighbors.push_back({&(blocs[y + 1][z][x]), UP_NEIGHBOUR});
	if (x > 0)
		neighbors.push_back({&(blocs[y][z][x - 1]), LEFT_NEIGHBOUR});
	if (x < CHUNK_WIDTH - 1)
		neighbors.push_back({&(blocs[y][z][x + 1]), RIGHT_NEIGHBOUR});
	if (z > 0)
		neighbors.push_back({&(blocs[y][z - 1][x]), BACK_NEIGHBOUR});
	if (z < CHUNK_DEPTH - 1)
		neighbors.push_back({&(blocs[y][z + 1][x]), FRONT_NEIGHBOUR});

	for (auto it : neighbors)
	{
		struct bloc *nei = it.first;
		GLuint currentFace = it.second;
		if (nei->type == NO_TYPE)
		{
			bloc->visible = true;
			visibleFaces |= currentFace;
		}
	}
	for (auto it : border_neighbors)
	{
		float max_height = it.first;
		GLuint currentFace = it.second;
		if (max_height < y)
		{
			bloc->visible = true;
			visibleFaces |= currentFace;
		}
	}

	bloc->faces = visibleFaces;

	return visibleFaces; // 127 or 63 ?
}

bool Chunk::generatePosOffsets(void)
{
	// TODO : Implement way of knowing which bloc should be shown to avoid loading
	//		too much data.
	// Should generate position of each bloc based on the chunk position
	Matrix mat;
	unsigned int i = 0;
	if (updateChunk)
	{
		GLfloat *WIP_transform = new GLfloat[spaceBorder[0].size() * 3]; //CHUNK_HEIGHT * CHUNK_WIDTH * CHUNK_DEPTH * 3];
		GLint *WIP_type = new GLint[spaceBorder[0].size()];
		/*for (int y = CHUNK_HEIGHT - 1; y >= 0; y--) // Too big of a loop
		{
			for (int z = CHUNK_DEPTH - 1; z >= 0; z--)
			{
				for (int x = CHUNK_WIDTH - 1; x >= 0; x--)
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
						//std::cout << WIP_transform[indexX] << ";";
						//std::cout << WIP_transform[indexY] << ";";
						//std::cout << WIP_transform[indexZ] << "\n";
					}
				}
			}
		}*/
		unsigned int indexX = 0;
		unsigned int indexY = 0;
		unsigned int indexZ = 0;
		for (auto blockVec: spaceBorder[0])
		{
			indexX = i * 3;
			indexY = i * 3 + 1;
			indexZ = i * 3 + 2;
			WIP_type[i] = blocs[blockVec.y][blockVec.z][blockVec.x].type;
			i += 1;
			WIP_transform[indexX] = position.x + blockVec.x;
			WIP_transform[indexY] = position.y + blockVec.y;
			WIP_transform[indexZ] = position.z + blockVec.z;
		}
		glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
		glBufferData(GL_ARRAY_BUFFER, spaceBorder[0].size() * 3 * sizeof(float),
					 WIP_transform, GL_STATIC_DRAW);
		delete[] WIP_transform;

		glBindBuffer(GL_ARRAY_BUFFER, typeVBO);
		glBufferData(GL_ARRAY_BUFFER, spaceBorder[0].size() * sizeof(GLint),
					 WIP_type, GL_STATIC_DRAW);
		delete[] WIP_type;

		if (spaceBorder[0].size() != facesToRender.size())
			std::cout << "Error : wtf problem with visible blocs and faces" << std::endl;
		glBindBuffer(GL_ARRAY_BUFFER, facesVBO);
		glBufferData(GL_ARRAY_BUFFER, facesToRender.size() * sizeof(GLuint),
					 facesToRender.data(), GL_STATIC_DRAW);

		updateChunk = false;
		return true;
	}
	return false;
}

bool Chunk::compareBlocs(const struct bloc *b1, const struct bloc *b2)
{
	return (b1->type == b2->type && b1->visible == b2->visible);
}

Chunk::bloc *Chunk::getVoxel(int x, int y, int z)
{
	return (&(blocs[y][z][x]));
}

void Chunk::greedyMesh(void)
{
	int i, j, k, l, w, h, u, v, n = 0;

	int x[] = {0, 0, 0};
	int dim[] = {CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_DEPTH};
	int q[] = {0, 0, 0};
	int du[] = {0, 0, 0};
	int dv[] = {0, 0, 0};

	struct bloc *mask[CHUNK_HEIGHT * CHUNK_WIDTH];

	/*
	 *	The variable backFace will be TRUE on first iteration and FALSE
	 *	on the second. This allows us to track which direction the indices
	 *	should run during the quad creation.
	 *
	 *	The loop will run twice and the inner one 3 times, each iteration will
	 *	be for one face of the voxel.
	 */
	for (bool backFace = true, b = false; b != backFace; backFace = backFace & b, b = !b)
	{
		// B1 and B2 are variables used for comparison;
		struct bloc *b1, *b2;

		// We check over 3 dimensions (x/y/z). Like explained by Mikola Lysenko.
		for (int d = 0; d < 3; d++)
		{
			// Variable VoxelFace
			u = (d + 1) % 3;
			v = (d + 2) % 3;

			x[0] = 0;
			x[1] = 0;
			x[2] = 0;

			q[0] = 0;
			q[1] = 0;
			q[2] = 0;
			q[d] = 1;

			// Sweep dimension from FRONT to BACK
			for (x[d] = -1; x[d] < dim[d];)
			{
				n = 0;
				// Mask computation

				for (x[v] = 0; x[v] < dim[v]; x[v]++)
					for (x[u] = 0; x[u] < dim[u]; x[u]++)
					{
						b1 = (x[d] >= 0) ? getVoxel(x[0], x[1], x[2]) : NULL;
						b2 = (x[d] < dim[d] - 1) ? getVoxel(x[0] + q[0], x[1] + q[1], x[2] + q[2]) : NULL;

						mask[n++] = ((b1 != NULL && b2 != NULL && compareBlocs(b1, b2))) ? NULL : backFace ? b1
																										   : b2;
					}
				x[d]++;

				n = 0;

				for (j = 0; j < CHUNK_HEIGHT; j++)
				{
					for (i = 0; i < CHUNK_WIDTH; i++)
					{
						if (mask[n] != NULL)
						{
							for (w = 1; i + w < CHUNK_WIDTH && mask[n + w] != NULL &&
										compareBlocs(mask[n + w], mask[n]);
								 w++)
							{
							}

							bool done = false;

							for (h = 1; j + h < CHUNK_HEIGHT; h++)
							{
								for (k = 0; k < w; k++)
								{
									if (mask[n + k + h * CHUNK_WIDTH] == NULL || !compareBlocs(mask[n + k + h * CHUNK_WIDTH], mask[n]))
									{
										done = true;
										break;
									}
								}
								if (done)
									break;
							}

							if (mask[n]->visible)
							{
								// Create quad
								x[u] = i;
								x[v] = j;

								du[0] = 0;
								du[1] = 0;
								du[2] = 0;
								du[u] = w;

								dv[0] = 0;
								dv[1] = 0;
								dv[2] = 0;
								dv[v] = h;

								createQuad(Vec3(x[0], x[1], x[2]),
										   Vec3(x[0] + du[0], x[1] + du[1], x[2] + du[2]),
										   Vec3(x[0] + du[0] + dv[0], x[1] + du[1] + dv[1], x[2] + du[2] + dv[2]),
										   Vec3(x[0] + dv[0], x[1] + dv[1], x[2] + dv[2]),
										   w,
										   h,
										   mask[n]);

								// Generate quad using found coordinates
							}

							for (l = 0; l < h; ++l)
								for (k = 0; k < w; ++k)
								{
									mask[n + k + l * CHUNK_WIDTH] = NULL;
								}

							i += w;
							n += w;
						}
						else
						{
							i++;
							n++;
						}
					}
				}
			}
		}
	}
}

void Chunk::createQuad(Vec3 bottomLeft, Vec3 topLeft, Vec3 topRight, Vec3 bottomRight, int width, int height,
					   Chunk::bloc *voxel)
{
	(void)bottomLeft, (void)bottomRight, (void)topLeft, (void)topRight;
	(void)width, (void)height;
	(void)voxel;
}

void Chunk::draw(Shader *shader)
{
	draw_safe.lock();
	if (hardBloc == 0 || hardBlocVisible == 0 || !init)
	{
		draw_safe.unlock();
		return;
	}
	//updateVisibilityByCamera();
	bool isThereNewData = Chunk::generatePosOffsets();
	(void)isThereNewData; // might be useful if we use multiple VAO

	//glBindTexture(GL_TEXTURE_CUBE_MAP, texture->getID());	// TODO : Modify in order to use more than one texture at once and those texture should not be loaded here but
	// in ResourceManager.
	//RectangularCuboid::drawInstance(shader, positionVBO, typeVBO, hardBlocVisible);
	//RectangularCuboid::drawFace(shader, positionVBO, typeVBO, hardBlocVisible, facesToRender);
	//RectangularCuboid::drawFaceInstance(shader, positionVBO, typeVBO, hardBlocVisible, facesVBO);
	RectangularCuboid::drawFaceInstance(shader, positionVBO, typeVBO, spaceBorder[0].size(), facesVBO);
	//RectangularCuboid::drawQuad(shader, positionVBO, typeVBO);

	draw_safe.unlock();
}

Vec2 Chunk::worldCoordToChunk(Vec3 worldPos)
{
	Vec2 pos;
	pos.x = floor(worldPos.x / CHUNK_WIDTH);
	pos.y = floor(worldPos.z / CHUNK_DEPTH);
	return pos;
}
