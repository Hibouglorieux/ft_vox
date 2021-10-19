#include "Chunk.hpp"

Chunk::Chunk(int x, int z)
{
	position = Vec3(x, 0, z);
	// Not necessary ?
	memset(blocs, 0, CHUNK_HEIGHT * CHUNK_WIDTH * CHUNK_DEPTH * sizeof(char));

	hardBloc = 0;
	// Generate perlin noise and set blocs
	// SET_BLOCS()
	heightMap = VoxelGenerator::createMap(0);
	texture = new Texture(heightMap);
	for(unsigned int y = 0; y < CHUNK_HEIGHT; y++)	// Too big of a loop
	{
		for(unsigned int x = 0; x < CHUNK_WIDTH; x++)
		{
			for(unsigned int z = 0; z < CHUNK_DEPTH; z++)
			{
				if (heightMap[x][z] * CHUNK_HEIGHT < y)
					blocs[y][x][z] = 0;
				else
				{
					blocs[y][x][z] = 1;
					hardBloc += 1;
				}
			}
		}
	}
}

Chunk::~Chunk(void)
{
	delete texture;
}

GLfloat *Chunk::generatePosOffsets(void)
{
	// TODO : Implement way of knowing which bloc should be shown to avoid loading
	//		too much data.
	GLfloat	*WIP_transform = new GLfloat[hardBloc * 3];//CHUNK_HEIGHT * CHUNK_WIDTH * CHUNK_DEPTH * 3];
	// Should generate position of each bloc based on the chunk position
	Matrix	mat;
	unsigned int i = 0;
	for(unsigned int y = 0; y < CHUNK_HEIGHT; y++)	// Too big of a loop
	{
		for(unsigned int x = 0; x < CHUNK_WIDTH; x++)
		{
			for(unsigned int z = 0; z < CHUNK_DEPTH; z++)
			{
				unsigned int index = (x + y * CHUNK_HEIGHT +
						z * CHUNK_HEIGHT * CHUNK_WIDTH + 0 * CHUNK_SIZE) * 3;
				unsigned int indexX = index + 0;
				unsigned int indexY = index + 1;
				unsigned int indexZ = index + 2;
				if (blocs[y][x][z] == 1)
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
			positionOffset, hardBloc);
	delete [] positionOffset;
}
