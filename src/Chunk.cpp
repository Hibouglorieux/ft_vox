#include "Chunk.hpp"

Chunk::Chunk(int x, int z)
{
	position = Vec3(x * CHUNK_WIDTH, 0, z * CHUNK_DEPTH);
	// Not necessary ?
	memset(blocs, 0, CHUNK_HEIGHT * CHUNK_WIDTH * CHUNK_DEPTH * sizeof(struct bloc));

	hardBloc = 0;
	struct bloc *bloc;
	// Generate perlin noise and set blocs
	// SET_BLOCS()
	heightMap = VoxelGenerator::createMap(0, x, z);
	texture = new Texture(heightMap);
	for(unsigned int y = 0; y < CHUNK_HEIGHT; y++)	// Too big of a loop
	{
		for(unsigned int x = 0; x < CHUNK_WIDTH; x++)
		{
			for(unsigned int z = 0; z < CHUNK_DEPTH; z++)
			{
				bloc = &(blocs[y][z][x]);
				if (heightMap[z][x] * CHUNK_HEIGHT < y)
				{
					bloc->type = 0;
					bloc->visible = 0;
					bloc->visibleInNextIteration = 0;
				}
				else
				{
					bloc->type = 1;
					bloc->visible = 1;
					bloc->visibleInNextIteration = 1;
					hardBloc += 1;
				}
				bloc->shouldUpdate = 0;
			}
		}
	}
	hardBlocVisible = 0;
	/*
	 *	Must check neighbor of a bloc to check if visible
	 *	For bloc at x/y/z :
	 *		- x + 1 / x - 1
	 *		- y + 1 / y - 1
	 *		- z + 1 / z - 1
	 *	If all neighbors block are visible then current block is not visible
	 *	If some neighbors are air then block is visible
	 */
	for(unsigned int y = 0; y < CHUNK_HEIGHT; y++)	// Too big of a loop
	{
		for(unsigned int x = 0; x < CHUNK_WIDTH; x++)
		{
			for(unsigned int z = 0; z < CHUNK_DEPTH; z++)
			{
				bloc = &(blocs[y][z][x]);
				SetVisibilityByNeighbors(x, y, z);
			}
		}
	}
	for(unsigned int y = 0; y < CHUNK_HEIGHT; y++)	// Too big of a loop
	{
		for(unsigned int x = 0; x < CHUNK_WIDTH; x++)
		{
			for(unsigned int z = 0; z < CHUNK_DEPTH; z++)
			{
				bloc = &(blocs[y][z][x]);
				if (bloc->shouldUpdate)
				{
					bloc->visible = bloc->visibleInNextIteration;
					bloc->shouldUpdate = false;
				}
				if (bloc->visible == true)
					hardBlocVisible += 1;
			}
		}
	}
	std::cout << "[CHUNK] Hard bloc count    : " << hardBloc << "\n";
	std::cout << "[CHUNK] Visible bloc count : " << hardBlocVisible  << "\n";
}

void Chunk::SetVisibilityByNeighbors(int x, int y, int z)
{
	// TODO : Check coordinate to avoid overflow
	struct bloc			*bloc = &(blocs[y][z][x]);
	std::vector<struct bloc*>	neighbors = {};
	long unsigned int			hardNei = 0;
	long unsigned int			invisibleNei = 0;

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
			break;
		}
		hardNei += bloc->visible == true ? 1 : 0;
		invisibleNei += bloc->visible == false ? 1 : 0;
	}
	if (hardNei == neighbors.size())
		bloc->visibleInNextIteration = false;
	else if (invisibleNei == neighbors.size())
		bloc->visibleInNextIteration = false;
	if (bloc->visibleInNextIteration != bloc->visible)
		bloc->shouldUpdate = true;
}

Chunk::~Chunk(void)
{
	delete texture;
}

GLfloat *Chunk::generatePosOffsets(void)
{
	// TODO : Implement way of knowing which bloc should be shown to avoid loading
	//		too much data.
	GLfloat	*WIP_transform = new GLfloat[hardBlocVisible * 3];//CHUNK_HEIGHT * CHUNK_WIDTH * CHUNK_DEPTH * 3];
	// Should generate position of each bloc based on the chunk position
	Matrix	mat;
	unsigned int i = 0;
	for(unsigned int y = 0; y < CHUNK_HEIGHT; y++)	// Too big of a loop
	{
		for(unsigned int x = 0; x < CHUNK_WIDTH; x++)
		{
			for(unsigned int z = 0; z < CHUNK_DEPTH; z++)
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
	return WIP_transform;
}

void Chunk::draw(Shader* shader)
{
	// Should draw instantiated bloc of same type once for each type.
	GLfloat	*positionOffset = Chunk::generatePosOffsets();

	//RectangularCuboid::drawInstance(pos, shader, texture,
	//		modelMatrices, CHUNK_SIZE);
	RectangularCuboid::drawInstance(shader, texture,
			positionOffset, hardBlocVisible);
	delete [] positionOffset;
}
