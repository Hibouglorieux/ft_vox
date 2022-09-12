/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VoxelGenerator.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/15 13:08:43 by nathan            #+#    #+#             */
/*   Updated: 2022/09/12 17:25:50 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef VOXELGENERATION_CLASS_H
# define VOXELGENERATION_CLASS_H
#include <array>
#include "ft_vox.h"
#include "Vec3.hpp"

enum blockType {
	NO_TYPE = 99,
	BLOCK_SAND = 0,
	BLOCK_DIRT = 1,
	BLOCK_GRASS,
	BLOCK_GRASS_SNOW,
	BLOCK_STONE,
	BLOCK_SNOW,
	BLOCK_BEDROCK
};

#define GRADIENT_SIZE 256 // will change in the future, it is 2pow(n) + 1 because
						// it's not yet looping on itself
						// bigger == bigger noise
						// is currently stretched to all chunk displayed as testing
#define HEIGHTMAP_SIZE BLOC_WIDTH_PER_CHUNK

#define BIG_HEIGHT_MAP_SIZE 1024
#define WORLEY_SIZE 256
#define WORLEY_Y_SIZE 128

typedef std::array<std::array<std::array<unsigned char, WIDTH>, LENGTH>, HEIGHT> VoxelMap;
typedef std::array<std::array<float, HEIGHTMAP_SIZE>, HEIGHTMAP_SIZE> HeightMap;
typedef std::array<std::array<std::array<float, HEIGHTMAP_SIZE>, HEIGHTMAP_SIZE>, 64> CaveMap;
typedef std::array<std::array<std::array<Vec3, WORLEY_SIZE>, WORLEY_SIZE>, WORLEY_Y_SIZE> WorleyGradient;
typedef std::array<std::array<float, BIG_HEIGHT_MAP_SIZE>, BIG_HEIGHT_MAP_SIZE> BigHeightMap;
typedef std::array<std::array<std::array<float, 2>, GRADIENT_SIZE>, GRADIENT_SIZE> Gradients;
//typedef std::array<std::array<std::array<float, HEIGHTMAP_SIZE>, HEIGHTMAP_SIZE>, HEIGHTMAP_SIZE> 3DHeightMap;

typedef std::array<Vec3, GRADIENT_SIZE> Gradient;

# define PERMUTATION_COUNT 3

typedef std::array<std::array<uint8_t, 512>, 3> permTable;
typedef std::array<std::array<Vec3, 512>, 3> gradTable; 

class VoxelGenerator {
public:
	static void				initialize(unsigned int seed);
	static void				initializeWorley(unsigned int seed);
	static void				clear();
	static BigHeightMap*	createBigMap(int octaves, float lacunarity, float gain);
	static HeightMap*		createMap(float ox, float oz, int octaves, float lacunarity, float gain);
	static CaveMap*			createCaveMap(float ox, float oz, int octaves, float lacunarity, float gain);
	static float			getLocalDensity(float x, float y, float z,
			float ox, float oy, float oz, int octaves,
			float lacunarity, float gain);

	static void				initialize(int seed, bool ok);
	static float			Noise2D(float x, float z, float output, float frequency, float amplitude, int octaves, int tableId, float lacunarity = 2.0f, float gain = 0.5f);
	static float			Noise3D(float x, float y, float z, float output, float frequency, float amplitude, int octaves, int tableId, float lacunarity = 2.0f, float gain = 0.5f, bool clamp = true);
	static float			getWorleyValueAt(float x, float y, float z);
private:
	static Gradients*	createPerlinGradient(unsigned int seed);
	static WorleyGradient* createWorleyGradient(unsigned int seed);
	static float dotGridGradient(int ix, int iy, float x, float y, Gradients* grad);
	static float dotGridGradient(int ix, int iy, int iz, float x, float y, float z, Gradients* grad);
	static float getValue(float x, float y, Gradients* grad);
	static float getValue(float x, float y, float z, Gradients* grad);
	static float lerp(float a0, float a1, float w);
	static float smoothstep(float t);
	static float quintic(float t);
	static float quinticDeriv(float t);
	static float seventic(float t);

	static int hash(int &x, int &y, int &z, int tableId);
	static float gradientDotV(int perm, float x, float y, float z);
	static float eval(float x, float y, float z, int tableId);

	static Gradients* gradients;
	static Gradients* gradients2;
	static WorleyGradient* worleyGradient;
	static Gradient* grad;
	static unsigned* permutationTable;

	//--------------------------------------------------------------------

	static permTable permsP;
	static gradTable gradsP;

	static bool tableInit;

	static void	initializePermutations(int seed, int tableId);
	static void shuffleTables(int seed, int tableId);

	static float simplexNoise2D(float x, float y, int tableId);
};

#endif
