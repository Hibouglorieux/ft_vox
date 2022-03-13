#include "Chunk.hpp"
#include <unistd.h>
#include <thread>
#include <algorithm>

#define TMP_SLEEP_VALUE 0.05 * SEC_TO_MICROSEC

#define CAVE_THRESHOLD 0.6f

int Chunk::totalChunks = 0;

#pragma region Constructor and init

Chunk::Chunk(int x, int z, Camera *camera)
{
	totalChunks++;
	position = Vec3(x * CHUNK_WIDTH, 0, z * CHUNK_DEPTH);
	//position.print();
	//std::cout << "Chunk : " << x << ",0," << z << std::endl;
	// Not necessary ?

	// Should we recompute the chunk's blocs positions
	updateChunk = false;

	init = false;
	threadUseCount = 1;

	spaceCount = 0;
	spaceBorder[0].clear();

	for (int face = 0; face < 4; face++)
		chunkFaces[face].clear();

	myNeighbours = {};
	playerCamera = camera;
	
	glGenBuffers(1, &typeVBO);
	glGenBuffers(1, &positionVBO);
	glGenBuffers(1, &facesVBO);
}

Chunk::Chunk(int x, int z, Camera *camera, std::vector<std::pair<Vec2, Chunk *>> neighbours) : Chunk(x, z, camera)
{
	std::vector<std::pair<Vec2, Chunk *>> _removed;
	std::vector<std::pair<Vec2, Chunk *>> _new;

	setNeighbors(neighbours);
}

Chunk::~Chunk(void)
{
	while (!hasThreadFinished()) { usleep(TMP_SLEEP_VALUE); } // Check if something is using our object.

	while (myNeighbours.size() != 0)
		deleteNeighbor(myNeighbours.back());
	while (faceNeighbor.size() != 0)
		faceNeighbor.clear();

	facesToRender.clear();
	//delete texture;
	glBindBuffer(GL_ARRAY_BUFFER, typeVBO);
	glDeleteBuffers(1, &typeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
	glDeleteBuffers(1, &positionVBO);
	//delete heightMap;
	totalChunks--;
}

float 	Chunk::getBlockBiome(int x, int z, bool setBlocInChunk)
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
	}
	else if (terrainBiomeValue < 0.45)
	{
		// Forest ?
	}

	if (!setBlocInChunk)
		return heightValue;

	struct bloc *bloc = &(blocs[(int)heightValue][z][x]);

	bloc->type = blocType;

	return heightValue;
}

void 	Chunk::initChunk(void)
{
	struct bloc *bloc;

	if (init)
		return;
	Vec2 _pos = getChunkPos();
	//printf("Chunk (%3i, %3i) starting init\n", _pos.x, _pos.y);

	blocs = BlocData();

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
				bloc->faces = 999;
				bloc->spaceId = -1;
				bloc->visited = false;
			}

		 	float blockValue = this->getBlockBiome(x, z);

		// 	TO DELETE 
			// if (blocs[blockValue][z][x].type != NO_TYPE)
			// {
			// 	spaceBorder[0].push_back(Vec3(x, (int)blockValue, z));
			// 	facesToRender.push_back(127);
			// }
		// 	END OF REGION TO DELETE

			if ((x == 0 || x == CHUNK_WIDTH - 1 || z == 0 || z == CHUNK_DEPTH - 1) && blocs[blockValue][z][x].type != NO_TYPE)	// TODO : Delete, this was to show chunk
				(&(blocs[blockValue][z][x]))->type = BLOCK_SAND;

			if (blockValue < min)
				min = blockValue;
			if (blockValue > max)
				max = blockValue;

			for (int j = (int)blockValue - 1; j > 0; j--)
			{
				bloc = &(blocs[j][z][x]);
				//int bloc_type = bloc->type;

				if ((&(blocs[(int)blockValue][z][x]))->type == BLOCK_GRASS || (&(blocs[(int)blockValue][z][x]))->type == NO_TYPE)
					bloc->type = BLOCK_DIRT;
				else
					bloc->type = BLOCK_STONE;
				bloc->visible = 0;

				if (true && j < blockValue && j > 0)
				{
					float noise = VoxelGenerator::Noise3D(position.x + x, j * 15.5, position.z + z, 0.25f, 0.0040f, 0.45f, 4, 0, 2.0, 0.5, true);

					if (noise > CAVE_THRESHOLD)
						bloc->type = NO_TYPE;
					else
						bloc->type = BLOCK_DIRT;
				}
				if ((x == 0 || x == CHUNK_WIDTH - 1 || z == 0 || z == CHUNK_DEPTH - 1) && bloc->type != NO_TYPE)	// TODO : Delete, this was to show chunk
					(&(blocs[j][z][x]))->type = BLOCK_SAND;
			}
		}
	}
	//std::cout << max << std::endl;
	if (max < CHUNK_HEIGHT)
		max = max + 30;
	boundingVolume = AABB(position, Vec3(position.x + CHUNK_WIDTH, max, position.z + CHUNK_DEPTH));
	//boundingVolume = AABB(Vec3(0, 0, 0), Vec3(CHUNK_WIDTH, max, CHUNK_DEPTH));
	//std::cout << "Is on frustum : " << boundingVolume.isOnFrustum(playerCamera->getFrustum()) << std::endl;

	setBlocsVisibility();
	//setBlocsSpace();

	init = true;

	//connectSpace();

	updateChunk = true;

	decreaseThreadCount();
}

#pragma endregion

#pragma region Neighbors Setup

void 	Chunk::setNeighbors(std::vector<std::pair<Vec2, Chunk*>> neighbours)
{
	myNeighbours = neighbours;
	//for (auto it = neighbours.begin(); it != neighbours.end(); ++it)
	//	addNeighbor((*it));
	//printf("N : %3li | F : %3li\n", myNeighbours.size(), faceNeighbor.size());
}

void	Chunk::addNeighbor(std::pair<Vec2, Chunk*> newNeighbour)
{
	Vec2 chnk_pos = getChunkPos();
	thread_safe.lock();
	std::vector<std::pair<Vec2, Chunk *>>::iterator index = find(myNeighbours.begin(), myNeighbours.end(), newNeighbour);
	if (index == myNeighbours.end())
		myNeighbours.push_back(newNeighbour);
	std::vector<std::pair<int, Chunk *>>::iterator indexFace = std::find_if(faceNeighbor.begin(), faceNeighbor.end(), [&newNeighbour](const std::pair<int, Chunk*>& element){ return element.second == newNeighbour.second;});
	if (indexFace == faceNeighbor.end())
	{
		int nx = newNeighbour.first.x - chnk_pos.x;
		int nz = newNeighbour.first.y - chnk_pos.y;
		if (nx < 0 || nx > 0)
			faceNeighbor.push_back(std::make_pair(nx < 0 ? WEST : EAST, newNeighbour.second));
		else if (nz < 0 || nz > 0)
			faceNeighbor.push_back(std::make_pair(nz < 0 ? SOUTH : NORTH , newNeighbour.second));
	}
	thread_safe.unlock();
}

void	Chunk::deleteNeighbor(std::pair<Vec2, Chunk*> neighbour)
{
	thread_safe.lock();
	std::vector<std::pair<Vec2, Chunk *>>::iterator index = find(myNeighbours.begin(), myNeighbours.end(), neighbour);
	std::vector<std::pair<int, Chunk *>>::iterator indexFace = std::find_if(faceNeighbor.begin(), faceNeighbor.end(), [&neighbour](const std::pair<int, Chunk*>& element){ return element.second == neighbour.second;});
	if (index != myNeighbours.end())
		myNeighbours.erase(index);
	if (indexFace != faceNeighbor.end())
		faceNeighbor.erase(indexFace);
	// TODO : delete all connected space in the deleted face
	thread_safe.unlock();
}

Chunk	*Chunk::getNeighborByFace(int face)
{
	Chunk *chnk = NULL;
	for (auto it = faceNeighbor.begin(); it != faceNeighbor.end(); ++it)
	{
		if (it->first == face)
		{
			chnk = it->second;
			break;
		}
	}
	return chnk;
}

#pragma endregion

#pragma region Visibility

std::vector<int>	Chunk::connectSpaceCall(int x, int y, int z, int face)
{
	(void)x;
	(void)y;
	(void)z;
	(void)face;
	return {};
}

void	Chunk::connectSpace(void)
{
	Chunk *neighbor = NULL;
	int spaceId = -1, face = -1, x, z;

	//printf("Size of face neighbors : %li\n", faceNeighbor.size());
	for (int face = 0; face < 4; face++)
	{
		neighbor = getNeighborByFace(face);
		//printf("%p\n", neighbor);
		if (neighbor == NULL)
			continue;
		//while (!(neighbor->init)) { usleep(TMP_SLEEP_VALUE); }
		for (auto it = chunkFaces[face].begin(); it != chunkFaces[face].end(); ++it)
		{
			spaceId = (*it);
			// Need to try each spaceId NO_TYPE block on the current face with neighbor
			for (int y = CHUNK_HEIGHT - 1; y > -1; y--)
			{
				switch (face)
				{
					case NORTH:
						/* code */
						break;
					case EAST:
						/* code */
						break;
					case WEST:
						/* code */
						break;
					case SOUTH:
						/* code */
						break;
					
					default:
						break;
				}
			}
		}
	}
}

void	Chunk::setBlocSpace(int x, int y, int z)
{
	bloc				*current_block = NULL;
	std::vector<Vec3> 	stack = {};
	Vec3				cur;

	stack.push_back(Vec3(x, y, z));
	while (!stack.empty())
	{
		cur = stack.back();
		stack.pop_back();
		current_block = &(blocs[cur.y][cur.z][cur.x]);

		if (current_block->visited)
			continue;
		current_block->visited = true;

		// Check if the bloc is of NO_TYPE type
		// If not, add it to the current border space (meaning visible filled block)
		if (current_block->type != NO_TYPE)
		{
			current_block->visible = true;
			if (current_block->faces == 999)
				setVisibilityByNeighbors(cur.x, cur.y, cur.z);

			/*if (cur.x == 0 || cur.x == CHUNK_WIDTH - 1)
				addFaceLink(spaceCount, cur.x == 0 ? WEST : EAST);
			if (cur.z == 0 || cur.z == CHUNK_DEPTH - 1)
				addFaceLink(spaceCount, cur.z == 0 ? SOUTH : NORTH);*/
			if (spaceCount == 0)
				facesToRender.push_back(current_block->faces);
			spaceBorder[spaceCount].push_back(cur);
			continue;
		}

		if (cur.x > 0)
			stack.push_back(Vec3(cur.x - 1, cur.y, cur.z));
		if (cur.x < CHUNK_WIDTH - 1)
			stack.push_back(Vec3(cur.x + 1, cur.y, cur.z));
		if (cur.z > 0)
			stack.push_back(Vec3(cur.x, cur.y, cur.z - 1));
		if (cur.z < CHUNK_DEPTH - 1)
			stack.push_back(Vec3(cur.x, cur.y, cur.z + 1));
		if (cur.y > 0)
			stack.push_back(Vec3(cur.x, cur.y - 1, cur.z));
		if (cur.y < CHUNK_HEIGHT - 1)
			stack.push_back(Vec3(cur.x, cur.y + 1, cur.z));
	}
}

void	Chunk::setBlocsSpace(void)
{
	/*for (unsigned int z = 0; z < CHUNK_DEPTH; z++)
	{
		for (unsigned int x = 0; x < CHUNK_WIDTH; x++)
		{
			for (unsigned int y = 0; y < CHUNK_HEIGHT; y++)
			{
				if (blocs[y][z][x].type != NO_TYPE)
					setVisibilityByNeighbors(x, y, z);
				else
					continue;
				//(&(blocs[y][z][x]))->visited = false;
			}
		}
	}*/
	for (int z = CHUNK_DEPTH - 1; z > -1; z--)
	{
		for (int x = CHUNK_WIDTH - 1; x > -1; x--)
		{
			for (int y = CHUNK_HEIGHT - 1; y > -1; y--)
			{
				if (blocs[y][z][x].type == NO_TYPE)
				{
					setBlocSpace(x, y, z);
					spaceCount++;
				}
				if (blocs[y][z][x].type != NO_TYPE)
					continue;
			}
		}
	}
}

void	Chunk::setBlocsVisibility(void)
{
	for (unsigned int z = 0; z < CHUNK_DEPTH; z++)
	{
		for (unsigned int x = 0; x < CHUNK_WIDTH; x++)
		{
			for (unsigned int y = 0; y < CHUNK_HEIGHT; y++)
			{
				if (blocs[y][z][x].type == NO_TYPE)
					continue;
				//setVisibilityByNeighbors(x, y, z);
				GLuint faces = setVisibilityByNeighbors(x, y, z);
				if (faces != 0)
				{
					spaceBorder[0].push_back(Vec3(x, y, z));
					facesToRender.push_back(faces);
				}
			}
		}
	}
}

GLuint	Chunk::setVisibilityByNeighbors(int x, int y, int z) // Activates visibility if one neighbour is transparent
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
	else if (x == CHUNK_WIDTH - 1)
	{
		border_neighbors.push_back({this->getBlockBiome(x + 1, z, false), RIGHT_NEIGHBOUR});
		border_neighbors_vec.push_back(Vec3(x + 1, y, z));
	}
	if (z == 0)
	{
		border_neighbors.push_back({this->getBlockBiome(x, z - 1, false), BACK_NEIGHBOUR});
		border_neighbors_vec.push_back(Vec3(x, y, z - 1));
	}
	else if (z == CHUNK_DEPTH - 1)
	{
		border_neighbors.push_back({this->getBlockBiome(x, z + 1, false), FRONT_NEIGHBOUR});
		border_neighbors_vec.push_back(Vec3(x, y, z + 1));
	}

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
	int i = 0;
	for (auto it : border_neighbors)
	{
		float max_height = it.first;
		GLuint currentFace = it.second;
		if (max_height < y || VoxelGenerator::Noise3D(position.x + border_neighbors_vec[i].x, y * 15.5, position.z + border_neighbors_vec[i].z, 0.25f, 0.0040f, 0.45f, 4, 0, 2.0, 0.5, true) > CAVE_THRESHOLD) // TODO : Need to know if neighbors is cave or not (so generate noise...)
		{
			bloc->visible = true;
			visibleFaces |= currentFace;
		}
		++i;
	}

	bloc->faces = visibleFaces;

	return visibleFaces;
}

#pragma endregion

#pragma region Display

bool 	Chunk::generatePosOffsets(void)
{
	// TODO : Implement way of knowing which bloc should be shown to avoid loading
	//		too much data.
	// Should generate position of each bloc based on the chunk position
	Matrix mat;
	unsigned int i = 0;
	if (updateChunk)
	{
		//printf("%li\n", ghostBorder[0].size());
		GLfloat *WIP_transform = new GLfloat[spaceBorder[0].size() * 3]; //CHUNK_HEIGHT * CHUNK_WIDTH * CHUNK_DEPTH * 3];
		GLint *WIP_type = new GLint[spaceBorder[0].size()];
		unsigned int indexX = 0;
		unsigned int indexY = 0;
		unsigned int indexZ = 0;
		for (auto blockVec: spaceBorder[0])
		{
			indexX = i * 3;
			indexY = i * 3 + 1;
			indexZ = i * 3 + 2;
			WIP_type[i] = blocs[blockVec.y][blockVec.z][blockVec.x].type;
			if (WIP_type[i] == 99)
				printf("ERROR, trying to display empty block\n");
			else if (WIP_type[i] < 0 && WIP_type[i] > 7 && WIP_type[i] != 99)
				printf("ERROR, trying to display unknown type\n");
			i += 1;
			WIP_transform[indexX] = position.x + blockVec.x;
			WIP_transform[indexY] = position.y + blockVec.y;
			WIP_transform[indexZ] = position.z + blockVec.z;
		}
		glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
		glBufferData(GL_ARRAY_BUFFER, (spaceBorder[0].size()) * 3 * sizeof(float),
					 WIP_transform, GL_STATIC_DRAW);
		delete[] WIP_transform;

		glBindBuffer(GL_ARRAY_BUFFER, typeVBO);
		glBufferData(GL_ARRAY_BUFFER, (spaceBorder[0].size()) * sizeof(GLint),
					 WIP_type, GL_STATIC_DRAW);
		delete[] WIP_type;

		if ((spaceBorder[0].size()) != facesToRender.size())
			std::cout << "Error : wtf problem with visible blocs and faces" << std::endl;
		glBindBuffer(GL_ARRAY_BUFFER, facesVBO);
		glBufferData(GL_ARRAY_BUFFER, facesToRender.size() * sizeof(GLuint),
					 facesToRender.data(), GL_STATIC_DRAW);

		updateChunk = false;
		return true;
	}
	return false;
}

void 	Chunk::draw(Shader *shader)
{
	draw_safe.lock();
	if (spaceBorder[0].size() == 0 || !init)
	{
		draw_safe.unlock();
		return;
	}
	bool isThereNewData = Chunk::generatePosOffsets();
	(void)isThereNewData; // might be useful if we use multiple VAO

	RectangularCuboid::drawFaceInstance(shader, positionVBO, typeVBO, spaceBorder[0].size(), facesVBO);

	draw_safe.unlock();
}

#pragma endregion

#pragma region Utilities
/*
** Utility of the CHUNK class 
 */

static inline float lerp(float a, float b, float f)
{
	return a + f * (b - a);
}

void	Chunk::addFaceLink(int spaceId, int face)
{
	bool valid_face = true;
	
	if (chunkFaces[face].size() == 0)
		valid_face = true;
	else
	{
		for (auto key = chunkFaces[face].begin(); key != chunkFaces[face].end(); ++key)
		{
			if (*key == spaceId)
			{
				valid_face = false;
				break;
			}
		}
	}
	if (valid_face)
		chunkFaces[face].push_back(spaceId);
}

Vec2 	Chunk::worldCoordToChunk(Vec3 worldPos)
{
	Vec2 pos;
	pos.x = floor(worldPos.x / CHUNK_WIDTH);
	pos.y = floor(worldPos.z / CHUNK_DEPTH);
	return pos;
}

#pragma endregion