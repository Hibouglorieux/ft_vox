#ifndef GREEDY_MESH_CLASS_H
# define GREEDY_MESH_CLASS_H

# include "Vec2.hpp"
# include "Vec3.hpp"
# include "Chunk.hpp"

GreedyMesh class {
	public:
		struct Quad		// This structure is use dto generate 2D quad
		{
			float x;
			float y;	//	(x,y) represent the upper-left corner of the quad
			float w;	//	w represent the width of the quad
			float h;	//	h represent the height of the quad
		};
	private:
		// Typedef
		typedef std::array<std::array<std::array<int, CHUNK_WIDTH>, CHUNK_DEPTH>, CHUNK_HEIGHT> BlocRegion;
		
		// Functions
		bool compareQuads(const struct Quad &q1, const struct Quad &q2);

		// Variables
		BlocRegion					region;
		std::vector<struct Quad>	quadList;
}

#endif
