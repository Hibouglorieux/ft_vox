#ifndef CHUNK_CLASS_H
# define CHUNK_CLASS_H

# include "Object.hpp"
# include "Shader.hpp"
# include "Texture.hpp"
# include "Matrix.hpp"
# include "RectangularCuboid.hpp"
# include <cstring>
# include "ft_vox.h"

# define CHUNK_HEIGHT 256 // HEIGHT

// defined in VoxelGenerator
# define CHUNK_WIDTH BLOC_WIDTH_PER_CHUNK
# define CHUNK_DEPTH BLOC_WIDTH_PER_CHUNK

# define CHUNK_SIZE CHUNK_HEIGHT * CHUNK_WIDTH * CHUNK_DEPTH


class Chunk : public Object{
public:
	Chunk(int x, int z);

	virtual ~Chunk(void);

	void	initChunk(void);
	virtual void	draw(Shader* shader) override;

	Vec3	getPos() const { return position; }
	void	chunkUpdateBloc(void);
	//char	*getBlocs() const { return blocs; }
private:
	struct bloc
	{
		int type;
		bool visible;
		bool shouldUpdate;
	};

	void	updateVisibility(void);
	void	setVisibilityByNeighbors(int x, int y, int z);
	GLfloat	*generatePosOffsets(void);

	Vec3	position;
	struct bloc	blocs[CHUNK_HEIGHT][CHUNK_WIDTH][CHUNK_DEPTH];
	GLfloat	*blocsPosition;
	bool	updateChunk;
	bool	init;
	HeightMap	heightMap;
	unsigned int hardBloc;
	unsigned int hardBlocVisible;
	Texture* texture;
};

#endif
