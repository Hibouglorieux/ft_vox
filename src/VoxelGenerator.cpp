/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VoxelGenerator.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/15 13:08:40 by nathan            #+#    #+#             */
/*   Updated: 2021/10/19 22:34:38 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "VoxelGenerator.hpp"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <random>


#define TMP_FIXED_GRADIENT_SIZE (GRADIENT_SIZE - 1)
#define SCALE_HEIGHTMAP_TO_FRACTION_OF_NOISE (1.f / HEIGHTMAP_SIZE * TMP_FIXED_GRADIENT_SIZE / (float)ROW_OF_CHUNK)

HeightMap	VoxelGenerator::createMap(unsigned long seed)
{
	PerlinNoise perlin(seed);
	HeightMap* myHeightMap = new HeightMap;
	for (int z = 0; z < HEIGHTMAP_SIZE; z++)
	{
		for (int x = 0; x < HEIGHTMAP_SIZE; x++)
		{
			(*myHeightMap)[z][x] = perlin.getValue((float)x * SCALE_HEIGHTMAP_TO_FRACTION_OF_NOISE, (float)z * SCALE_HEIGHTMAP_TO_FRACTION_OF_NOISE);
			float* tmp = &(*myHeightMap)[z][x];
			*tmp = (*tmp + 1) / 2;
		}
	}
	return *myHeightMap;
}

HeightMap	VoxelGenerator::createMap(unsigned long seed, int ox, int oz)
{
	PerlinNoise perlin(seed);
	ox *= TMP_FIXED_GRADIENT_SIZE / ROW_OF_CHUNK;
	oz *= TMP_FIXED_GRADIENT_SIZE / ROW_OF_CHUNK;
	HeightMap* myHeightMap = new HeightMap;
	for (int z = oz; z < HEIGHTMAP_SIZE + oz; z++)
	{
		for (int x = ox; x < HEIGHTMAP_SIZE + ox; x++)
		{	
			(*myHeightMap)[z - oz][x - ox] = perlin.getValue(ox + (float)(x - ox) * SCALE_HEIGHTMAP_TO_FRACTION_OF_NOISE, oz + (float)(z - oz) * SCALE_HEIGHTMAP_TO_FRACTION_OF_NOISE);
			float* tmp = &(*myHeightMap)[z - oz][x - ox];
			*tmp = (*tmp + 1) / 2;
		}
	}
	return *myHeightMap;
}

// Function to linearly interpolate between a0 and a1
// Weight w should be in the range [0.0, 1.0]

float	PerlinNoise::lerp(float a0, float a1, float w) const
{
	return (1.0 - w)*a0 + w*a1;
}

PerlinNoise::PerlinNoise(unsigned long seed)
{
	std::mt19937 generator(seed);
	std::uniform_real_distribution<> distribution(0.f, 1.f);
	auto dice = [&generator, &distribution](){return distribution(generator);};
	float gradientLength;
	for (int i = 0; i < GRADIENT_SIZE; i++)
	{

		for (int j = 0; j < GRADIENT_SIZE; j++)
		{
			gradientLength = 2;
			while (gradientLength > 1)
			{
				gradients[j][i][0] = 2 * dice() - 1;
				float& x = gradients[j][i][0];
				gradients[j][i][1] = 2 * dice() - 1;
				float& y = gradients[j][i][1];
				gradientLength = x * x + y * y;
			}
			gradientLength= sqrtf(gradientLength);
			gradients[j][i][0] /= gradientLength;
			gradients[j][i][1] /= gradientLength;
		}
	}
}

// Computes the dot product of the distance and gradient vectors.
float PerlinNoise::dotGridGradient(int ix, int iy, float x, float y)
{

	// Precomputed (or otherwise) gradient vectors at each grid node

	// Compute the distance vector
	float dx = x - (float)ix;
	float dy = y - (float)iy;

	// Compute the dot-product
	return (dx * gradients[iy][ix][0] + dy * gradients[iy][ix][1]);
}

// Compute Perlin noise at coordinates x, y
float	PerlinNoise::getValue(float x, float y)
{

	// Determine grid cell coordinates
	int x0 = (int)floor(x);
	int x1 = x0 + 1;
	int y0 = (int)floor(y);
	int y1 = y0 + 1;

	// Determine interpolation weights
	// Could also use higher order polynomial/s-curve here
	float sx = x - (float)x0;
	float sy = y - (float)y0;

	// Interpolate between grid point gradients
	float n0, n1, ix0, ix1, value;
	n0 = dotGridGradient(x0, y0, x, y);
	n1 = dotGridGradient(x1, y0, x, y);
	ix0 = lerp(n0, n1, sx);
	n0 = dotGridGradient(x0, y1, x, y);
	n1 = dotGridGradient(x1, y1, x, y);
	ix1 = lerp(n0, n1, sx);
	value = lerp(ix0, ix1, sy);

	return value;
}
