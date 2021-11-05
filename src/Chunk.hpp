#ifndef CHUNK_CLASS_H
# define CHUNK_CLASS_H

# include "Object.hpp"
# include "Shader.hpp"
# include "Texture.hpp"
# include "Matrix.hpp"
# include "RectangularCuboid.hpp"
# include <cstring>
# include "ft_vox.h"
# include "Vec2.hpp"
# include <functional>
# include <mutex>

# define CHUNK_HEIGHT 64 //HEIGHT

# define CHUNK_WIDTH BLOC_WIDTH_PER_CHUNK
# define CHUNK_DEPTH BLOC_WIDTH_PER_CHUNK

# define CHUNK_SIZE CHUNK_HEIGHT * CHUNK_WIDTH * CHUNK_DEPTH

# define WATER_LEVEL 10

class Chunk : public Object{
public:
	Chunk(int x, int z);
	Chunk(int x, int z, std::vector<std::pair<Vec2, Chunk*>> neighbours);

	void	initChunk(void);

	virtual ~Chunk(void);

	bool	hasThreadFinished(void) {return threadUseCount == 0;}
	void	increaseThreadCount(void) {threadUseCount++;}
	virtual void	draw(Shader* shader) override;

	Vec3	getPos() const { return position; }
	Vec3	getChunkCenterPos() const { return (position + Vec3(CHUNK_WIDTH / 2, 0, CHUNK_DEPTH / 2)); }

	static	Vec2 worldCoordToChunk(Vec3 worldPos);
	static	int totalChunks;
	//char	*getBlocs() const { return blocs; }
private:
	struct bloc
	{
		int type;
		bool visible;
	};
typedef std::array<std::array<std::array<struct bloc, CHUNK_WIDTH>, CHUNK_DEPTH>, CHUNK_HEIGHT> BlocData;

	void	updateVisibilityWithNeighbour(Vec2 NeighbourPos,
			const BlocData& neighbourBlocs,
			std::function<void(const BlocData&)> callBack = nullptr);
	void	updateVisibility(void);
	void	setVisibilityByNeighbors(int x, int y, int z);
	void	caveTest();
	void	destroyIlots();
	GLfloat	*generatePosOffsets(void);

	Vec3	position;
	BlocData blocs;
	//struct bloc	blocs[CHUNK_HEIGHT][CHUNK_DEPTH][CHUNK_WIDTH];
	GLfloat	*blocsPosition;
	bool	updateChunk;
	bool	init;
	unsigned char	threadUseCount;
	HeightMap*	heightMap;
	CaveMap*	caveMap;
	unsigned int hardBloc;
	unsigned int hardBlocVisible;
	std::mutex	draw_safe;
	Texture* texture;

	std::vector<std::pair<Vec2, Chunk*>> myNeighbours;
	Chunk	*forward;
	Chunk	*right;
	Chunk	*left;
	Chunk	*backward;
};

#endif
