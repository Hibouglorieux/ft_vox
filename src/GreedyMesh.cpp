#include "GreedyMesh.hpp"

bool compareBlocs(const struct bloc *b1, const struct bloc *b2)
{
	return (b1->type == b2->type && b1->visible == b2->visible);
}

struct bloc *getVoxel(int x, int y, int z)
{
	return (&(blocs[y][z][x]));
}

GreedyMesh::GreedyMesh(void)
{
	int i, j, k, l, w, h, u, v, n = 0;

	int x[]		= {0, 0, 0};
	int dim[]	= {CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_DEPTH};
	int q[]		= {0, 0, 0};
	int du[]	= {0, 0, 0};
	int dv[]	= {0, 0, 0};

	struct bloc *mask[] = new struct *bloc[CHUNK_HEIGHT * CHUNK_WIDTH];

	/*
	 *	The variable backFace will be TRUE on first iteration and FALSE
	 *	on the second. This allows us to track which direction the indices
	 *	should run during the quad creation.
	 *
	 *	The loop will run twice and the inner one 3 times, each iteration will
	 *	be for one face of the voxel.
	 */
	for (bool backFace = true, b = false; b != backFace; backFace = backFace & b, b != !b)
	{
		// B1 and B2 are variables used for comparison;
		struct bloc *b1, *b2;

		// We check over 3 dimensions (x/y/z). Like explained by Mikola Lysenko.
		for (int d = 0; d < 3; d++)
		{
			// Variable VoxelFace
			u = (d + 1) % 3;
			v = (d + 2) % 3;

			x[0] = 0;
			x[1] = 0;
			x[2] = 0;

			q[0] = 0;
			q[1] = 0;
			q[2] = 0;
			q[d] = 1;

			// Sweep dimension from FRONT to BACK
			for (x[d] = -1; x[d] < dim[d];)
			{
				n = 0;
				// Mask computation

				for (x[v] = 0; x[v] < CHUNK_HEIGHT; x[v]++)
					for (x[u] = 0; x[u] < CHUNK_WIDTH; x[u]++)
					{
						b1 = (x[d] > 0) ?
							getVoxel(x[0], x[1], x[2]) : NULL;
						b2 = (x[d] < CHUNK_WIDTH - 1) ?
							getVoxel(x[0] + q[0], x[1] + q[1], x[2] + q[2]) : NULL;

						mask[n++] = ((b1 != NULL && b2 != NULL
									&& compareBlocs(b1, b2))) ? NULL : backFace ? b1 : b2;
					}
			}

			x[d]++;

			n = 0;

			for (j = 0; j < CHUNK_HEIGHT; j++)
			{
				for (i = 0; i < CHUNK_WIDTH; i++)
				{
					if (mask[n] != NULL)
					{
						for (w = 1; i + w < CHUNK_WIDTH && mask[n + w] != NULL &&
								compareBlocs(mask[n + w], mask[n]); w++)
						{}

						bool done = false;

						for (h = 1; j + h < CHUNK_HEIGHT; h++)
						{
							for (k = 0; k < w; k++)
							{
								if (mask[n + k + h * CHUNK_WIDTH] == NULL
										|| !compareBlocs(mask[n + k + h * CHUNK_WIDTH], mask[n]))
								{
									done = true;
									break;
								}
							}
							if (done)
								break;
						}

						if (!mask[n].visible)
						{
							// Create quad
							x[u] = i;
							x[v] = j;

							du[0] = 0;
							du[1] = 0;
							du[2] = 0;
							du[u] = w;

							dv[0] = 0;
							dv[1] = 0;
							dv[2] = 0;
							dv[v] = h;

							// Generate quad using found coordinates
						}

						for (l = 0; l < h; ++l)
							for (k = 0; k < w; ++k)
							{
								mask[n + k + l * CHUNK_WIDTH] = NULL;
							}

						i += w;
						n += w;
					}
					else
					{
						i++;
						n++;
					}
				}
			}
		}
	}
	delete mask;
}

void GreedyMesh::convertDataToRegion()
{
}

bool GreedyMesh::compareQuads(const struct Quad &q1, const struct Quad &q2)
{
	if (q1.y != q2.y) return q1.y < q2.y;
	if (q1.x != q2.x) return q1.x < q2.x;
	if (q1.w != q2.w) return q1.w > q2.w;
	return q1.h >= q2.h;
}

void GreedyMesh::generateGreedyMesh(void)
{
	// Sources :	0fps.net meshing in a minecraft game
	//				Jason Gedge - Greedy Voxel Meshing

	// Greedy Mesh should be used only once but when interacting with the world
	// we might need to desconstruct meshes to separate blocs then reconstruct
	// new quads (in case of block destruction or adding).

	// This goal of greedy meshing is to fuse block to draw less vertices.
	// To do so, we need to look for a block and try to the biggest quad possible
	// To make the biggest quad we must have rules :
	//	1 - All block which will make the quad must be connected (2D)
	//	2 - All block connected must be of same type
	//	3 - Will be a quad and not a random polygon

	// To determine quad :
	//	- Find first block on y layer that satisfy type != NO_TYPE mark as corner of quad
	//	- Find all connected block on z axis of same type, mark last as corner of quad
	//	- Find all connected block on x axis of same type, mark last as corner of quad
	// Note : REMEMBER that shape must be a quad

	// Rather than looping in my blocs list, I need to transform it in a region map
	// in which I will be able to delete part of it. I will then generate quad
	// till the region is not empty.

	// The region will be a 3D array of the blocs type. When used,
	// the value inside the array will be replaced by a negative number, which means that this
	// part was already processed. This negative number will be the id of quad
	// (negative) in case we need to look for it.

	int currentBlock = -1;
	Vec3 quad[4] = {};
	for (int y = CHUNK_HEIGHT - 1; y > -1; y--)
		for (int z = 0; z < CHUNK_DEPTH; z++)
			for (int x = 0; x < CHUNK_WIDTH; x++)
			{
				if (currentBlock == -1 && blocs[y][z][x].type == NO_TYPE)
					continue;
				if (currentBlock == -1) // First block with type that we found is the start of our quad
				{
					currentBlock = blocs[y][z][x].type;
					continue;
				}
				else
				{
					if (currentBlock != blocs[y][z][x].type) // The current progress over the line is over.
				}
			}
}

