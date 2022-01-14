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
# include "Biome.hpp"
# include "Vec2.hpp"
# include <functional>
# include <mutex>
# include <memory>
# include <algorithm>

# define CHUNK_HEIGHT HEIGHT

# define CHUNK_WIDTH BLOC_WIDTH_PER_CHUNK
# define CHUNK_DEPTH BLOC_WIDTH_PER_CHUNK

# define CHUNK_SIZE CHUNK_HEIGHT * CHUNK_WIDTH * CHUNK_DEPTH

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
	AABB	boundingVolume = AABB(Vec3(0, 0, 0), Vec3(CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_DEPTH));

	void	updateVisibilityByCamera(bool freeze);

private:
	struct bloc
	{
		GLint type;
		bool visible;
		GLint spaceId;
	};

	struct compare
	{
		Vec3 key;
		compare(const Vec3 &i):key(i) {}

		bool operator()(const Vec3 &i) {
			return (i == key);
		}
	};

	typedef std::array<std::array<std::array<struct bloc, CHUNK_WIDTH>, CHUNK_DEPTH>, CHUNK_HEIGHT> BlocData;
	typedef std::array<std::array<std::array<int, CHUNK_WIDTH>, CHUNK_DEPTH>, CHUNK_HEIGHT> BlocSearchData;
	typedef std::array<std::pair<GLint, std::vector<struct bloc *>>, CHUNK_SIZE> BlocSpaceBorder;

	void	updateVisibilityWithNeighbour(Vec2 NeighbourPos,
			const BlocData& neighbourBlocs,
			std::function<void(const BlocData&)> callBack = nullptr);
	bool	updateVis = false;
	void	updateVisibility(void);
	GLuint	setVisibilityByNeighbors(int x, int y, int z);

	// Space Functions
	void	generateConnectedBlocList(int x, int y, int z, std::vector<Vec3> *connectedBlocPos, std::vector<Vec3> *visitedBlocsMaster);
	void	generateConnectedSpaces(void);
	
	void	updateVisibilitySpace(void);
	// End of Space Functions

	// Greedy Meshing Functions
	bool		compareBlocs(const struct bloc *b1, const struct bloc *b2);
	struct bloc *getVoxel(int x, int y, int z);
	void		greedyMesh();
	void		createQuad(Vec3 bottomLeft, Vec3 topLeft, Vec3 topRight, Vec3 bottomRight, int width, int height, Chunk::bloc *voxel);
	// End of GM Functions

	void	getQuads();
	bool	generatePosOffsets();

	Vec3	position;
	BlocData blocs;
	BlocSearchData blocsTests;

	int					spaceCount;
	BlocSpaceBorder		spaceBorder;
	std::array<int, CHUNK_SIZE> spaceESize;

	bool	updateChunk;
	bool	init;
	unsigned char	threadUseCount;
	HeightMap*	heightMap;					// TODO : Delete, Not useful anymore
	CaveMap*	caveMap;					// TODO : Delete, not useful
	unsigned int hardBloc;
	unsigned int hardBlocVisible;
	std::mutex	draw_safe;
	Texture* texture;						// TODO : Delete

	GLuint typeVBO, positionVBO, facesVBO;
	std::vector<GLuint>	facesToRender;

	std::vector<std::pair<Vec2, Chunk*>> myNeighbours;

	float	getBlockBiome(int x, int z, bool setBlocInChunk = true);
	//float	getBlockBiome(int x, int z, bool setBlocInChunk = true, bool superFlat = false);

	Camera	*playerCamera;
};

#endif
