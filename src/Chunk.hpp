#ifndef CHUNK_CLASS_H
# define CHUNK_CLASS_H

# include "Camera.hpp"
# include "Object.hpp"
# include "Shader.hpp"
# include "Texture.hpp"
# include "Matrix.hpp"
# include "RectangularCuboid.hpp"
# include "VoxelGenerator.hpp"
# include <cstring>
# include "ft_vox.h"
# include "Vec2.hpp"
# include <functional>
# include <mutex>

# define CHUNK_HEIGHT HEIGHT

# define CHUNK_WIDTH BLOC_WIDTH_PER_CHUNK
# define CHUNK_DEPTH BLOC_WIDTH_PER_CHUNK

# define CHUNK_SIZE CHUNK_HEIGHT * CHUNK_WIDTH * CHUNK_DEPTH

# define WATER_LEVEL 32

class Chunk : public Object{
public:
	Chunk(int x, int z, Camera *camera);
	Chunk(int x, int z, Camera *camera, std::vector<std::pair<Vec2, Chunk*>> neighbours);

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
		GLint type;
		bool visible;
	};
	typedef std::array<std::array<std::array<struct bloc, CHUNK_WIDTH>, CHUNK_DEPTH>, CHUNK_HEIGHT> BlocData;
	typedef std::array<std::array<std::array<int, CHUNK_WIDTH>, CHUNK_DEPTH>, CHUNK_HEIGHT> BlocSearchData;

	void	updateVisibilityWithNeighbour(Vec2 NeighbourPos,
			const BlocData& neighbourBlocs,
			std::function<void(const BlocData&)> callBack = nullptr);
	void	updateVisibilityByCamera(void);
	void	updateVisibility(void);
	void	setVisibilityByNeighbors(int x, int y, int z);
	void	caveTest();
	void	worleyCaveTest();
	void	destroyIlots();
	bool 	destroyIlotsSearchAndDestroy(struct bloc *block, Vec3 pos, std::vector<struct bloc*> *blockGroup);
	bool	generatePosOffsets();

	Vec3	position;
	BlocData blocs;
	BlocSearchData blocsTests;
	//struct bloc	blocs[CHUNK_HEIGHT][CHUNK_DEPTH][CHUNK_WIDTH];
	bool	updateChunk;
	bool	init;
	unsigned char	threadUseCount;
	HeightMap*	heightMap;
	CaveMap*	caveMap;
	unsigned int hardBloc;
	unsigned int hardBlocVisible;
	std::mutex	draw_safe;
	Texture* texture;

	GLuint typeVBO, positionVBO;

	std::vector<std::pair<Vec2, Chunk*>> myNeighbours;

	float	getBlockBiome(int x, int z);

	Camera	*playerCamera;
};

#endif
