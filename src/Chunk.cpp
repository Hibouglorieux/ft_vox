#include "Chunk.hpp"
#include <unistd.h>
#include "TextManager.hpp"

#define TMP_SLEEP_VALUE 0.05 * SEC_TO_MICROSEC

#define DEBUG_CONDITION (position == Vec3(5 * CHUNK_WIDTH, 0, -1 * CHUNK_DEPTH) && x == 0 && y == 72 && z == 0)
#undef DEBUG_CONDITION
#define DEBUG_CONDITION false

#define CAVE_THRESHOLD 0.6f

int Chunk::totalChunks = 0;

Chunk::Chunk(int x, int z, Camera *camera, const std::map<Vec2, std::vector<Vec3>>& blocsDeleted)
{
	totalChunks++;
	position = Vec3(x * CHUNK_WIDTH, 0, z * CHUNK_DEPTH);
	//position.print();
	//std::cout << "Chunk : " << x << ",0," << z << std::endl;
	// Not necessary ?

	deletedBlocs = &blocsDeleted;
	// Should we recompute the chunk's blocs positions
	updateChunk = false;

	// How many solid bloc in chunk
	hardBloc = 0;
	hardBlocVisible = 0;
	init = false;
	threadUseCount = 1;

	spaceCount = 0;
	spaceESize = { 0 };

	myNeighbours = {};
	playerCamera = camera;
	glGenBuffers(1, &allVBO);
}

Chunk::Chunk(int x, int z, Camera *camera, std::vector<std::pair<Vec2, Chunk *>> neighbours, const std::map<Vec2, std::vector<Vec3>>& blocsDeleted) : Chunk(x, z, camera, blocsDeleted)
{
	(void)neighbours;
	myNeighbours = neighbours;
}

void	Chunk::updateWithNeighbourBlockDestroyed(Vec3 blocAffected)
{
	struct bloc& bloc = blocs[blocAffected.y][blocAffected.z][blocAffected.x];
	if (bloc.type != NO_TYPE)
		updateChunk = true;
}

bool	Chunk::deleteBlock(Vec3 blockToTest)
{
	//std::cout << "chunkTest at " << blockToTest << std::endl;
	if (std::find(spaceBorder[0].begin(), spaceBorder[0].end(), blockToTest) == spaceBorder[0].end())
	{
		/*
		for (auto it : spaceBorder[0])
			if (it.x == blockToTest.x)
				std::cout << it;
				*/
		return false;
	}
	else
	{
		//std::cout << "HIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIT! " << std::endl;
		blocs[blockToTest.y][blockToTest.z][blockToTest.x].type = NO_TYPE;
		updateChunk = true;
		spaceBorder[0].clear();
		facesToRender.clear();
		updateVisibility();
		//generatePosOffsets();
	}
	return true;
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
	//terrainBiomeValue = 0.0;
	//heightValue = 1;

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
		else
			blocType = BLOCK_GRASS_SNOW;
	}
	else if (terrainBiomeValue < 0.30)
	{
		//blocType = BLOCK_GRASS_SNOW;
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

bool Chunk::hasBlockBeenDestroyed(Vec3 blocPos)
{
	Vec2 pos2d = Chunk::worldCoordToChunk(position);
	if (blocPos.x < 0)
		pos2d.x--;
	if (blocPos.x > CHUNK_WIDTH)
		pos2d.x++;
	if (blocPos.z < 0)
		pos2d.y--;
	if (blocPos.z > CHUNK_DEPTH)
		pos2d.y++;
	if (deletedBlocs->find(pos2d) == deletedBlocs->end())
		return false;
	const std::vector<Vec3>& myDeletedBlocs= deletedBlocs->at(pos2d);
	if (std::find(myDeletedBlocs.begin(), myDeletedBlocs.end(), blocPos) == myDeletedBlocs.end())
		return false;
	return true;
}

void Chunk::initChunk(void)
{
	struct bloc *bloc;

	if (init)
		return;

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
					bloc->type = BLOCK_BEDROCK;
				else
					bloc->type = NO_TYPE;
				bloc->visible = false;
				bloc->spaceId = -1;
				bloc->visited = false;
			}

			float blockValue = this->getBlockBiome(x, z);

			if (hasBlockBeenDestroyed(Vec3(x, (int)blockValue, z)))
			{// undo what was done in getBlockBiome
				struct bloc* tmpBloc = &blocs[(int)blockValue][z][x];
				tmpBloc->type = NO_TYPE;
				tmpBloc->visible = false;
				hardBloc--;
				hardBlocVisible--;
			}
			//if ((x == 0 || x == CHUNK_WIDTH - 1 || z == 0 || z == CHUNK_DEPTH - 1) && blocs[blockValue][z][x].type != NO_TYPE)	// TODO : Delete, this was to show chunk
			//	(&(blocs[blockValue][z][x]))->type = BLOCK_SAND;

			if (blockValue < min)
				min = blockValue;
			if (blockValue > max)
				max = blockValue;

			for (int j = (int)blockValue - 1; j > 0; j--)
			{
				bloc = &(blocs[j][z][x]);
				if (hasBlockBeenDestroyed(Vec3(x, j, z)))
				{
					bloc->type = NO_TYPE;
					continue;
				}
				//int bloc_type = bloc->type;

				if ((&(blocs[(int)blockValue][z][x]))->type == BLOCK_GRASS || (&(blocs[(int)blockValue][z][x]))->type == NO_TYPE)
					bloc->type = BLOCK_DIRT;
				else
					bloc->type = BLOCK_STONE;
				bloc->visible = 0;
				hardBloc++;

				if (DEBUG_CONDITION && j == 3 && z == 0 && x == 1)
					printf("My block has type: %d\n", bloc->type);
				/*
				if (true && j < blockValue && j > 0)
				{
					float noise = VoxelGenerator::Noise3D(position.x + x, j * 15.5, position.z + z, 0.25f, 0.0040f, 0.45f, 4, 0, 2.0, 0.5, true);

					if (noise > CAVE_THRESHOLD)
						bloc->type = NO_TYPE;
					else
						bloc->type = BLOCK_DIRT;
						//bloc->type = BLOCK_DIRT;
				}
				*/
				//if ((x == 0 || x == CHUNK_WIDTH - 1 || z == 0 || z == CHUNK_DEPTH - 1) && bloc->type != NO_TYPE)	// TODO : Delete, this was to show chunk
				//	(&(blocs[j][z][x]))->type = BLOCK_SAND;
			}
		}
	}
	doWorleyCaves();
	//std::cout << max << std::endl;
	if (max < CHUNK_HEIGHT)
		max = max + 30;
	boundingVolume = AABB(position, Vec3(position.x + CHUNK_WIDTH, max, position.z + CHUNK_DEPTH));
	//boundingVolume = AABB(Vec3(0, 0, 0), Vec3(CHUNK_WIDTH, max, CHUNK_DEPTH));
	//std::cout << "Is on frustum : " << boundingVolume.isOnFrustum(playerCamera->getFrustum()) << std::endl;

	updateVisibility();
	//updateVisibilitySpace();

	init = true;

	updateChunk = true;

	Vec2 myPos = worldCoordToChunk(getPos());
	threadUseCount = 0;
}

#define MAX_DEPTH_FOR_CAVED 3
#define TMP_FLOOR_LEVEL 20
#define TMP_WORLEY_Y_DIVISOR (float)(TMP_FLOOR_LEVEL - MAX_DEPTH_FOR_CAVED)
bool	Chunk::isBlockEmptyAfterWorley(float x, float y, float z)
{
	float xScaled = x / (float)(CHUNK_WIDTH - 1) + position.x / CHUNK_WIDTH;
	float yScaled = y / TMP_WORLEY_Y_DIVISOR;
	float zScaled = z / (float)(CHUNK_DEPTH - 1) + position.z / CHUNK_DEPTH;
	//xScaled *= 0.5f;
	//zScaled *= 0.5f;
	float worleyValue = VoxelGenerator::getWorleyValueAt(xScaled, yScaled, zScaled);
	/*
	if (position == Vec3(0 * CHUNK_WIDTH, 0, -1 * CHUNK_DEPTH) || position == Vec3(0, 0, -2 * CHUNK_DEPTH))
		printf("%f\n", worleyValue);
		*/
	if (worleyValue >= 0.75f)
		return true;
	return false;
}

void	Chunk::doWorleyCaves()
{
	struct bloc	*bloc;
	for(unsigned int y = MAX_DEPTH_FOR_CAVED; y < TMP_FLOOR_LEVEL; y++)// random number, can't dig lower than floor_level, cant dig higher than floor // TODO replace with FLOOR_LEVEL
		for(unsigned int x = 0; x < CHUNK_WIDTH; x++)
			for(unsigned int z = 0; z < CHUNK_DEPTH; z++)
			{
				//if (y >= )// TODO add condition here to make it not being able to make a hole through a mountain (keep it below floor/water level ?). Also needs coherent noise that doesnt start at 0.6f of CHUNK_HEIGHT as minimum result
					//continue;
				bloc = &(blocs[y][z][x]);
				/*
				if (bloc->type != NO_TYPE && (position == Vec3(0 * CHUNK_WIDTH, 0, -1 * CHUNK_DEPTH) || position == Vec3(0, 0, -2 * CHUNK_DEPTH)))
					printf("for Chunk at x: %d z: %d worley value at x:%d y:%d z:%d => ", (int)position.x / CHUNK_WIDTH, (int)position.z / CHUNK_DEPTH, x, y, z);
					*/
				if (bloc->type != NO_TYPE && isBlockEmptyAfterWorley(x, y, z))
					{
						// destroy the block
						bloc->type = NO_TYPE;
						bloc->visible = false;
						hardBloc--;
					}
			}
}

Chunk::~Chunk(void)
{
	glBindBuffer(GL_ARRAY_BUFFER, allVBO);
	glDeleteBuffers(1, &allVBO);
	totalChunks--;
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
				if (bloc->type == NO_TYPE)
					continue;
				GLuint faces = setVisibilityByNeighbors(x - 1, y - 1, z - 1);
				if (faces != 0)
				{
					facesToRender.push_back(faces);
					spaceBorder[0].push_back(Vec3(x - 1, y - 1, z - 1));
				}
			}
		}
	}
}

/**
 * @brief Function to add blocks at the border of the chunk to the current space.
 * Compute neighbors height to work.
 * 
 * @param x x coordinate (in chunk)
 * @param y y coordinate (in chunk)
 * @param z z coordinate (in chunk)
 * @param xHeight getBlockBiome output for (x +- 1, y, z)
 * @param zHeight getBlockBiome output for (x, y, z +- 1)
 * @param master should the function be updating downward (y) blocks
 */
void Chunk::updateVisibilityBorder(int x, int y, int z, int xHeight, int zHeight, bool master)
{
	int maxDiff = 0, face = 0;
	/*float noiseX = 10, noiseZ = 10;
	bool ghost = false;*/
	Vec3 pos;

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

	// noiseX/Z get the noise of the 'outside' block
	// If value >= CAVE_THRESHOLD, the 'outside' block is AIR (so no ghost ?)
	/* To know if current block need ghost face we must know :
	*		- current block is empty (NO_TYPE)
	*		- 'outside'/border block is filled (!= NO_TYPE)
	*
	*	PROBLEMS :
	*		- Might duplicate faces on already displayed blocks in neighbors
	*		- Require more work to work correctly with player interactions 
	*/

	//noiseX = VoxelGenerator::Noise3D(position.x + (x == 0 ? x - 1 : x + 1), y * 15.5, position.z + z, 0.25f, 0.0040f, 0.45f, 4, 0, 2.0, 0.5, true);
	//noiseZ = VoxelGenerator::Noise3D(position.x + x, y * 15.5, position.z + (z == 0 ? z - 1 : z + 1), 0.25f, 0.0040f, 0.45f, 4, 0, 2.0, 0.5, true);
	// If noiseX or noiseZ is higher than 0.6f, it means that the block is empty.
	// If block is empty then no need for ghost faces.

	// If both height are superior to current y, no block under us need to be displayed
	//if ((xHeight >= y && zHeight >= y))// && (noiseX < CAVE_THRESHOLD && noiseZ < CAVE_THRESHOLD))
	//	return;

	/*
	*	Each corner (0, 0), (1, 0), (0, 1), (1, 1) can have two ghost,
	*   any other part of the boundary can have at most one ghost.
	*/

	struct bloc *currentBlock = &(blocs[y][z][x]);
	/*	PROBLEM : This will create datarace on space merge ID.
	*	Situation :
	*		- let say we have chunk a, chunk b and chunk c
	*		- chunk a call chunk b and both have a merge in progress
	*		- chunk c is calling chunk a and find that he needs to merge space BUT
	*		that space is currently being merged with b (a/b)
	*/
	if (currentBlock->type == NO_TYPE)
	{
		face = x == 0 ? WEST : x == CHUNK_WIDTH - 1 ? EAST : 0;
		if (face != 0)
		{
			/*	Do something with the face based on X
			* 	Call neighbors in the needed direction to check its block at :
			*		x = x +- 1 | y = y | z = z +- 1
			*	If neighbor block is solid, we must display it.
			*	Though only displaying it won't be enough, we will have to link spaces.
			*
			*	Using face (NORTH/EAST/SOUTH/WEST) we are able to know which neighbor is needed.
			*   We then send them the position of the block we are looking at, with that the neighbor will check its own block.
			*	If the block is empty, the neighbor will have to link the block space with our space, for that we need to generate a Unique Identifier.
			*/
			updateVisibilityBorderWithNeighbors(x + (face == WEST ? -1 : face == EAST ? 1 : 0), y, z, face);
		}
		face = z == 0 ? SOUTH : z == CHUNK_DEPTH - 1 ? NORTH : 0;
		if (face != 0)
			updateVisibilityBorderWithNeighbors(x, y, z + (face == SOUTH ? -1 : face == NORTH ? 1 : 0), face);
	}

	/*if ((x == 0 || x == CHUNK_WIDTH - 1) && (z == 0 || z == CHUNK_DEPTH - 1)) // corners
	{
		if ((x == 0 || x == CHUNK_WIDTH - 1) && noiseX < CAVE_THRESHOLD && blocs[y][z][x].type == NO_TYPE)
			ghostBloc.faces |= x != 0 ? LEFT_NEIGHBOUR : RIGHT_NEIGHBOUR;
		if ((z == 0 || z == CHUNK_DEPTH - 1) && noiseZ < CAVE_THRESHOLD && blocs[y][z][x].type == NO_TYPE)
			ghostBloc2.faces |= z != 0 ? BACK_NEIGHBOUR : FRONT_NEIGHBOUR;
		if (ghostBloc.faces != 0 || ghostBloc2.faces != 0)
		{
			ghost = true;
			while (ghostBorder.size() < (long unsigned)(spaceCount + 1))
				ghostBorder.push_back({});
		}
		if (ghostBloc.faces != 0)
			ghostBorder[spaceCount].push_back({ghostBloc, Vec3((x == 0 ? x - 1 : x + 1), y, z)});
		if (ghostBloc2.faces != 0)
			ghostBorder[spaceCount].push_back({ghostBloc2, Vec3(x, y, (z == 0 ? z - 1 : z + 1 ))});
	}
	else // the rest
	{
		if ((x == 0 || x == CHUNK_WIDTH - 1) && noiseX < CAVE_THRESHOLD && blocs[y][z][x].type == NO_TYPE)
			ghostBloc.faces |= x != 0 ? LEFT_NEIGHBOUR : RIGHT_NEIGHBOUR;
		else if ((z == 0 || z == CHUNK_DEPTH - 1) && noiseZ < CAVE_THRESHOLD && blocs[y][z][x].type == NO_TYPE)
			ghostBloc.faces |= z != 0 ? BACK_NEIGHBOUR : FRONT_NEIGHBOUR;
		if (ghostBloc.faces != 0)
		{
			while (ghostBorder.size() < (long unsigned)(spaceCount + 1))
				ghostBorder.push_back({});
			if (x == 0 || x == CHUNK_WIDTH - 1)
				ghostBorder[spaceCount].push_back({ghostBloc, Vec3((x == 0 ? x - 1 : x + 1), y, z)});
			else if (z == 0 || z == CHUNK_DEPTH - 1)
				ghostBorder[spaceCount].push_back({ghostBloc, Vec3(x, y, (z == 0 ? z - 1 : z + 1 ))});
			ghost = true;
		}
	}*/
	
	GLuint faces = 0;
	if ((xHeight > -1 && xHeight < y))
		faces |= x == 0 ? LEFT_NEIGHBOUR : RIGHT_NEIGHBOUR;
	if ((zHeight > -1 && zHeight < y))
		faces |= z == 0 ? BACK_NEIGHBOUR : FRONT_NEIGHBOUR;

	if (faces == 0 /*&& !ghost */&& !master)
		return;

	if (blocs[y][z][x].type != NO_TYPE && faces != 0)
	{
		facesToRender.push_back(faces);
		spaceBorder[spaceCount].push_back(Vec3(x, y, z));
	}

	if (master)
	{
		if (xHeight > -1 && zHeight > -1)
			maxDiff = xHeight > zHeight ? zHeight : xHeight;
		else if (xHeight > -1 || zHeight > -1)
			maxDiff = xHeight > -1 ? xHeight : zHeight;
		maxDiff = 1;
		for (int yDiff = 1; y - yDiff > maxDiff; yDiff++)
			updateVisibilityBorder(x, y - yDiff, z, xHeight, zHeight);
	}
}

/**
 * @brief Function which will ask the neighbor, selected using the face,
 * the state of its block at x/y/z. Following state, we will update the spaces accordingly.
 * 
 * @param x x coordinate (in chunk)
 * @param y y coordinate (in chunk)
 * @param z z coordinate (in chunk)
 */
void Chunk::updateVisibilityBorderWithNeighbors(int x, int y, int z, int face)
{
	(void)x;
	(void)y;
	(void)z;
	Chunk	*chnk = NULL;
	Vec2	cur = Vec2();
	if (myNeighbours.size() == 0)
		return;	// Not good. I should not be here in the first place.
	chnk = myNeighbours[0].second;
	cur = myNeighbours[0].first;
	for (auto it : myNeighbours)
	{
		if (face == NORTH || face == SOUTH)
			chnk = face == NORTH ? (it.first.y > cur.y ? it.second : chnk) : (it.first.y < cur.y ? it.second : chnk);
		else if (face == EAST || face == WEST)
			chnk = face == EAST  ? (it.first.x > cur.x ? it.second : chnk) : (it.first.x < cur.x ? it.second : chnk);
	}
	if (!chnk)
		return;
	// Call the fateful neighbors for check.
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

	// While the stack is not empty, look for neighbors
	int p = 0;
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
		p++;
		(&(blocs[cur.y][cur.z][cur.x]))->visited = true;
		// Check if the bloc is of NO_TYPE type
		// If not, add it to the current border space
		if (blocs[cur.y][cur.z][cur.x].type != NO_TYPE)
		{
			if (spaceCount == 0) // Why ? Because current var is incorrect if using multiple spaces. Must change that to have a facesToRender[spaceCount]
				facesToRender.push_back(blocs[cur.y][cur.z][cur.x].faces);
			spaceBorder[spaceCount].push_back(cur);
			//if (cur.y > 0 && (cur.x == 0 || cur.x == CHUNK_WIDTH - 1 || cur.z == 0 || cur.z == CHUNK_DEPTH - 1))
			//	updateVisibilityBorder(cur.x, cur.y - 1, cur.z, -1, -1, true);
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
	//printf("%i vs %i -> %s\n", p, CHUNK_SIZE, p == CHUNK_SIZE ? "True" : "False");
	//printf("%i\n", spaceBlocCount);
	spaceESize[spaceCount] += spaceBlocCount;
}

/**
 * @brief Iterate through the NO_TYPE blocks to separate them in spaces.
 * Each space will be composed of solid blocks which enclosing unconnected space.
 * If two space where to be in contact, only the largest would be left after merging with the smallest.
 */
void Chunk::updateVisibilitySpace(void)
{
	struct bloc *bloc;
	
	spaceESize[spaceCount] = 0;
	for (int y = CHUNK_HEIGHT; y > 0; y--)
	{
		for (int z = CHUNK_DEPTH; z > 0; z--)
		{
			for (int x = CHUNK_WIDTH; x > 0; x--)
			{
				bloc = &(blocs[y - 1][z - 1][x - 1]);
				if (bloc->type == NO_TYPE)
				{
					if (bloc->spaceId != spaceCount && bloc->spaceId != -1) // Compare size of both space and keep the biggest one (merge ecount + borders (keep unique))
					{
						//bool fbigger = spaceESize[spaceCount] > spaceESize[bloc->spaceId];
						// TODO : implement that
					}
					else if (bloc->spaceId == -1) // Set current spaceId and propagate it to neighbors then increase spaceId
					{
						updateVisibilitySpaceAux(x - 1, y - 1, z - 1);
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
	std::vector<Vec3> border_neighbors_vec = {};
	GLuint visibleFaces = 0;

	// Check with neighbors case using noise
	if (x == 0)
	{
		border_neighbors.push_back({this->getBlockBiome(x - 1, z, false), LEFT_NEIGHBOUR});
		border_neighbors_vec.push_back(Vec3(x - 1, y, z));
	}
	else
		neighbors.push_back({&(blocs[y][z][x - 1]), LEFT_NEIGHBOUR});

	if (x == CHUNK_WIDTH - 1)
	{
		border_neighbors.push_back({this->getBlockBiome(x + 1, z, false), RIGHT_NEIGHBOUR});
		border_neighbors_vec.push_back(Vec3(x + 1, y, z));
	}
	else
		neighbors.push_back({&(blocs[y][z][x + 1]), RIGHT_NEIGHBOUR});

	if (z == 0)
	{
		border_neighbors.push_back({this->getBlockBiome(x, z - 1, false), BACK_NEIGHBOUR});
		border_neighbors_vec.push_back(Vec3(x, y, z - 1));
	}
	else
		neighbors.push_back({&(blocs[y][z - 1][x]), BACK_NEIGHBOUR});

	if (z == CHUNK_DEPTH - 1)
	{
		border_neighbors.push_back({this->getBlockBiome(x, z + 1, false), FRONT_NEIGHBOUR});
		border_neighbors_vec.push_back(Vec3(x, y, z + 1));
	}
	else
		neighbors.push_back({&(blocs[y][z + 1][x]), FRONT_NEIGHBOUR});

	if (y > 0)
		neighbors.push_back({&(blocs[y - 1][z][x]), BOTTOM_NEIGHBOUR});
	if (y < CHUNK_HEIGHT - 1)
		neighbors.push_back({&(blocs[y + 1][z][x]), UP_NEIGHBOUR});

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

	if (DEBUG_CONDITION)
	{
		std::cout << "for x == 1 and z == " << z << " i have " << border_neighbors.size() << " neighbours" << std::endl;
	}
	
	int i = 0;
	for (auto it : border_neighbors)
	{
		float max_height = it.first;
		GLuint currentFace = it.second;
		bool isBlockEmpty = isBlockEmptyAfterWorley(border_neighbors_vec[i]);
		if ((isBlockEmpty && max_height < y)|| max_height < y || hasBlockBeenDestroyed(border_neighbors_vec[i]))
		{
			bloc->visible = true;
			visibleFaces |= currentFace;
		}
		++i;
		if (DEBUG_CONDITION)
		{
			std::cout << "for z == " << z << " i get blockempty: " << isBlockEmpty << " and maxheight == " << max_height << " and y == " << y << std::endl;
		}
	}
	if (DEBUG_CONDITION)
		printf("value for block:%d, visibility:%d, faces:%d\n", bloc->type, (int)bloc->visible, visibleFaces);


	bloc->faces |= visibleFaces;

	return visibleFaces; // 127 or 63 ?
}

bool Chunk::generatePosOffsets(void)
{
	// TODO : Implement way of knowing which bloc should be shown to avoid loading
	//		too much data.
	// Should generate position of each bloc based on the chunk position
	unsigned int i = 0;
	if (updateChunk)
	{
		GLfloat *vboData = new GLfloat[spaceBorder[0].size() * (5)];
		unsigned int indexX = 0;
		unsigned int indexY = 0;
		unsigned int indexZ = 0;
		for (auto blockVec: spaceBorder[0])
		{
			indexX = i * 5;
			indexY = i * 5 + 1;
			indexZ = i * 5 + 2;
			unsigned int type = i * 5 + 3;
			unsigned int face = i * 5 + 4;
			//WIP_type[i] = blocs[blockVec.y][blockVec.z][blockVec.x].type;
			//if (WIP_type[i] == 99)
				//printf("ERROR, trying to display empty block\n");
			//else if (WIP_type[i] < 0 && WIP_type[i] > 7 && WIP_type[i] != 99)
				//printf("ERROR, trying to display unknown type\n");
			vboData[indexX] = position.x + blockVec.x;
			vboData[indexY] = position.y + blockVec.y;
			vboData[indexZ] = position.z + blockVec.z;
			vboData[type] = static_cast<GLfloat>(blocs[blockVec.y][blockVec.z][blockVec.x].type);
			vboData[face] = static_cast<GLfloat>(facesToRender[i]);
			i++;
		}
		glBindBuffer(GL_ARRAY_BUFFER, allVBO);
		glBufferData(GL_ARRAY_BUFFER, spaceBorder[0].size() * 5 * sizeof(float),
					 vboData, GL_STATIC_READ);
		delete[] vboData;
		updateChunk = false;
		return true;
	}
	return false;
}

void Chunk::draw(Shader *shader)
{
	if (spaceBorder[0].size() == 0 || !init)
		return;
	/*
	if (position == Vec3(1 * CHUNK_WIDTH, 0, 1 * CHUNK_DEPTH))
		std::cout << "in interesting chunk i have " << hardBlocVisible << " hardbloc visible" << std::endl;
		*/
	//updateVisibilityByCamera();
	
	generatePosOffsets();// generates or updates buffer only if needed

	RectangularCuboid::drawFaceInstance(shader, allVBO, spaceBorder[0].size());
}

Vec2 Chunk::worldCoordToChunk(Vec3 worldPos)
{
	Vec2 pos;
	pos.x = floor((worldPos.x + 0.5)/ CHUNK_WIDTH);
	pos.y = floor((worldPos.z + 0.5)/ CHUNK_DEPTH);
	return pos;
}
