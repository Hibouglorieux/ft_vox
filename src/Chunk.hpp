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

# define NORTH	0
# define EAST	1
# define SOUTH	2
# define WEST	3

class Chunk : public Object{
public:
	Chunk(int x, int z, Camera *camera);
	Chunk(int x, int z, Camera *camera, std::vector<std::pair<Vec2, Chunk*>> neighbours);

	void	initChunk(void);
	void	setNeighbors(std::vector<std::pair<Vec2, Chunk*>> neighbours);// { myNeighbours = neighbours; }
	void	addNeighbor(std::pair<Vec2, Chunk*> newNeighbour);
	void	deleteNeighbor(std::pair<Vec2, Chunk*> neighbor);
	std::vector<std::pair<Vec2, Chunk*>>	getNeighbors(void) { return myNeighbours; }

	virtual ~Chunk(void);

	bool	hasThreadFinished(void) {return threadUseCount == 0;}
	bool	getThreadCount(void) {return threadUseCount;}
	void	increaseThreadCount(void) { thread_safe.lock(); threadUseCount++; /*Vec2 bob = getChunkPos(); printf("(%3i,%3i) %i UP\n", bob.x, bob.y, threadUseCount);*/ thread_safe.unlock(); }
	void	decreaseThreadCount(void) { thread_safe.lock(); threadUseCount > 0 ? threadUseCount-- : threadUseCount = 0; /*Vec2 bob = getChunkPos(); printf("(%3i,%3i) %i DN\n", bob.x, bob.y, threadUseCount);*/ thread_safe.unlock(); }
	virtual void	draw(Shader* shader) override;

	Vec3	getPos() const { return position; }
	Vec2	getChunkPos() const { return worldCoordToChunk(position); }
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
	typedef std::array<std::vector<std::pair<int, int>>, CHUNK_SIZE> SpaceConnectedness;
	typedef std::array<std::vector<int>, 4> ChunkSpaceFaces;

	Chunk	*getNeighborByFace(int face);


	std::vector<int>	connectSpaceCall(int x, int y, int z, int face);
	void	connectSpace();
	void	setBlocsVisibility();
	GLuint	setVisibilityByNeighbors(int x, int y, int z);
	void	setBlocSpace(int x, int y, int z);
	void	setBlocsSpace();

	void	addFaceLink(int spaceId, int face);

	bool	generatePosOffsets();

	Vec3			position;
	BlocData		blocs;

	bool			updateChunk = true;
	bool			init		= false;
	unsigned char 	threadUseCount;
	std::mutex		draw_safe;
	std::mutex		thread_safe;

	int					spaceCount;
	BlocSpaceBorder		spaceBorder;
	SpaceConnectedness	spaceLinks;
	ChunkSpaceFaces		chunkFaces;

	GLuint typeVBO, positionVBO, facesVBO;
	std::vector<GLuint>	facesToRender;

	std::vector<std::pair<Vec2, Chunk*>> myNeighbours; // Should have at most 4 neigbhors
	std::vector<std::pair<int, Chunk*>> faceNeighbor;

	float	getBlockBiome(int x, int z, bool setBlocInChunk = true);

	Camera	*playerCamera;
};

#endif
