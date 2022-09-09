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

# define NORTH	1
# define EAST	2
# define SOUTH	3
# define WEST	4

class Chunk : public Object{
public:
	Chunk(int x, int z, Camera *camera);
	Chunk(int x, int z, Camera *camera, std::vector<std::pair<Vec2, Chunk*>> neighbours);

	void	initChunk(void);
	void	setNeighbors(std::vector<std::pair<Vec2, Chunk*>> neighbours) { myNeighbours = neighbours; }
	std::vector<std::pair<Vec2, Chunk*>>	getNeighbors(void) { return myNeighbours; }

	virtual ~Chunk(void);

	bool	hasThreadFinished(void) {return threadUseCount == 0;}
	virtual void	draw(Shader* shader) override;

	Vec3	getPos() const { return position; }
	Vec3	getChunkCenterPos() const { return (position + Vec3(CHUNK_WIDTH / 2, 0, CHUNK_DEPTH / 2)); }

	static	Vec2 worldCoordToChunk(Vec3 worldPos);
	static	int totalChunks;
	AABB	boundingVolume = AABB(Vec3(0, 0, 0), Vec3(CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_DEPTH));
private:
	struct bloc
	{
		GLint 	type;
		bool 	visible;
		GLint 	spaceId;
		bool 	visited;
		GLuint	faces;
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
	typedef std::array<std::vector<Vec3>, CHUNK_SIZE> BlocSpaceBorder;

	bool	isBlockEmptyAfterWorley(float x, float y, float z);
	bool	isBlockEmptyAfterWorley(Vec3 coords) { return isBlockEmptyAfterWorley(coords.x, coords.y, coords.z);}
	void	doWorleyCaves();
	void	updateVisibilityWithNeighbour(Vec2 NeighbourPos,
			const BlocData& neighbourBlocs,
			std::function<void(const BlocData&)> callBack = nullptr);
	bool	updateVis = false;
	void	updateVisibility(void);
	GLuint	setVisibilityByNeighbors(int x, int y, int z);

	// Space Functions
	void	updateVisibilityBorder(int x, int y, int z, int xHeight = -1, int zHeight = -1, bool master = false);
	void	updateVisibilityBorderWithNeighbors(int x, int y, int z, int face);
	void	updateVisibilitySpaceAux(int x, int y, int z);
	void	updateVisibilitySpace(void);


	void	getQuads();
	bool	generatePosOffsets();

	Vec3	position;
	BlocData blocs;

	int											spaceCount;
	BlocSpaceBorder								spaceBorder;
	std::array<int, CHUNK_SIZE> 				spaceESize;

	bool			updateChunk = true;
	bool			init		= false;
	unsigned char 	threadUseCount;
	unsigned int 	hardBloc;
	unsigned int 	hardBlocVisible;

	GLuint allVBO;
	std::vector<GLuint>	facesToRender;

	std::vector<std::pair<Vec2, Chunk*>> myNeighbours; // Should have at most 4 neigbhors

	float	getBlockBiome(int x, int z, bool setBlocInChunk = true);
	//float	getBlockBiome(int x, int z, bool setBlocInChunk = true, bool superFlat = false);

	Camera	*playerCamera;



};

#endif
