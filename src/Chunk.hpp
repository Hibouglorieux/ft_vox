#ifndef CHUNK_CLASS_H
# define CHUNK_CLASS_H

# define CHUNK_HEIGHT	16	// 256
# define CHUNK_WIDTH	64	// 16
# define CHUNK_DEPTH	64	// 16
# define CHUNK_SIZE CHUNK_HEIGHT * CHUNK_WIDTH * CHUNK_DEPTH

# include "Object.hpp"
# include "Shader.hpp"
# include "Texture.hpp"
# include "Matrix.hpp"
# include "RectangularCuboid.hpp"
# include <cstring>

class Chunk : public Object{
public:
	Chunk(int x, int z);

	virtual ~Chunk(void);

	virtual void	draw(Shader* shader) override;

	Vec3	getPos() const { return position; }
	//char	*getBlocs() const { return blocs; }
private:

	GLfloat	*generatePosOffsets(void);
	Vec3	position;
	char	blocs[CHUNK_HEIGHT][CHUNK_WIDTH][CHUNK_DEPTH];
	HeightMap	heightMap;
	unsigned int hardBloc;
	Texture* texture;

};

#endif
