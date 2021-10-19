#ifndef CHUNK_CLASS_H
# define CHUNK_CLASS_H

# define CHUNK_HEIGHT	16	// 256
# define CHUNK_WIDTH	16	// 16
# define CHUNK_DEPTH	16	// 16
# define CHUNK_SIZE CHUNK_HEIGHT * CHUNK_WIDTH * CHUNK_DEPTH

# include "Object.hpp"
# include "Texture.hpp"
# include "Matrix.hpp"
# include "RectangularCuboid.hpp"
# include <cstring>

class Chunk {
public:
	Chunk(int x, int z);

	void	draw(Matrix *viewMat, Vec3& pos, Shader* shader, Texture* texture);

	Vec3	getPos() const { return position; }
	//char	*getBlocs() const { return blocs; }
private:

	GLfloat	*generateMatrices(void);
	Vec3	position;
	char	blocs[CHUNK_HEIGHT][CHUNK_WIDTH][CHUNK_DEPTH];
	HeightMap	heightMap;
	unsigned int hardBloc;
};

#endif
