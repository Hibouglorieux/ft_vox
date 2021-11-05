/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VoxelGenerator.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/15 13:08:43 by nathan            #+#    #+#             */
/*   Updated: 2021/11/05 15:02:41 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef VOXELGENERATION_CLASS_H
# define VOXELGENERATION_CLASS_H
#include <array>
#include "ft_vox.h"
#include "Vec3.hpp"

#define GRADIENT_SIZE 256 // will change in the future, it is 2pow(n) + 1 because
						// it's not yet looping on itself
						// bigger == bigger noise
						// is currently stretched to all chunk displayed as testing
#define HEIGHTMAP_SIZE BLOC_WIDTH_PER_CHUNK

#define BIG_HEIGHT_MAP_SIZE 1024

typedef std::array<std::array<std::array<unsigned char, WIDTH>, LENGTH>, HEIGHT> VoxelMap;
typedef std::array<std::array<float, HEIGHTMAP_SIZE>, HEIGHTMAP_SIZE> HeightMap;
typedef std::array<std::array<std::array<float, HEIGHTMAP_SIZE>, HEIGHTMAP_SIZE>, 64> CaveMap;
typedef std::array<std::array<float, BIG_HEIGHT_MAP_SIZE>, BIG_HEIGHT_MAP_SIZE> BigHeightMap;
typedef std::array<std::array<std::array<float, 2>, GRADIENT_SIZE>, GRADIENT_SIZE> Gradients;
//typedef std::array<std::array<std::array<float, HEIGHTMAP_SIZE>, HEIGHTMAP_SIZE>, HEIGHTMAP_SIZE> 3DHeightMap;

typedef std::array<Vec3, GRADIENT_SIZE> Gradient;

class VoxelGenerator {
public:
	static void				initialize(unsigned int seed);
	static void				clear();
	static HeightMap*		createMap();
	static BigHeightMap*	createBigMap();
	static BigHeightMap*	createBigMap(int octaves, float lacunarity, float gain);
	static HeightMap*		createMap(float ox, float oz);
	static HeightMap*		createMap(float ox, float oz, int octaves, float lacunarity, float gain);
	static CaveMap*			createCaveMap(float ox, float oz, int octaves, float lacunarity, float gain);
	static float			getLocalDensity(float x, float y, float z,
			float ox, float oy, float oz, int octaves,
			float lacunarity, float gain);
private:
	static Gradients*	createPerlinGradient(unsigned int seed);
	static float dotGridGradient(int ix, int iy, float x, float y, Gradients* grad);
	static float dotGridGradient(int ix, int iy, int iz, float x, float y, float z, Gradients* grad);
	static float getValue(float x, float y, Gradients* grad);
	static float getValue(float x, float y, float z, Gradients* grad);
	static float lerp(float a0, float a1, float w);
	static float smoothstep(float t);
	static float quintic(float t);
	static float quinticDeriv(float t);
	static float seventic(float t);

	static int hash(int &x, int &y, int &z);
	static float gradientDotV(int perm, float x, float y, float z);
	static float eval(float x, float y, float z);

	static Gradients* gradients;
	static Gradients* gradients2;
	static Gradient* grad;
	static unsigned* permutationTable;
};

#endif
