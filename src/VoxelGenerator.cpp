/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VoxelGenerator.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/15 13:08:40 by nathan            #+#    #+#             */
/*   Updated: 2021/10/21 15:46:44 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "VoxelGenerator.hpp"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <random>

#define SCALE_HEIGHTMAP_TO_FRACTION_OF_NOISE ((float)1.0 / HEIGHTMAP_SIZE * GRADIENT_SIZE / (float)MAX_NB_OF_CHUNK)

#define MAX_LAYER 1
Gradients* VoxelGenerator::gradients = nullptr;

void VoxelGenerator::Initialize(unsigned long seed)
{
	if (gradients == nullptr)
		createPerlinGradient(seed);
}

HeightMap	VoxelGenerator::createMap()
{
	HeightMap* myHeightMap = new HeightMap;
	for (int z = 0; z < HEIGHTMAP_SIZE; z++)
	{
		for (int x = 0; x < HEIGHTMAP_SIZE; x++)
		{
			(*myHeightMap)[z][x] = getValue((float)x * SCALE_HEIGHTMAP_TO_FRACTION_OF_NOISE, (float)z * SCALE_HEIGHTMAP_TO_FRACTION_OF_NOISE);
			float* tmp = &(*myHeightMap)[z][x];
			*tmp = (*tmp + 1) / 2;
		}
	}
	return *myHeightMap;// TODO Waw, nice leak bro
}

BigHeightMap	VoxelGenerator::createBigMap()
{
	BigHeightMap* myBigHeightMap = new BigHeightMap;
	for (int z = 0; z < BIG_HEIGHT_MAP_SIZE; z++)
	{
		for (int x = 0; x < BIG_HEIGHT_MAP_SIZE; x++)
		{
#define TMP_BIG_HEIGHT_MAP_SCALE ((float)1.0 / (float)BIG_HEIGHT_MAP_SIZE * (float)GRADIENT_SIZE / 16.0)
			int maxLayer = MAX_LAYER;
			float& tmp = (*myBigHeightMap)[z][x];
			float tmpx = x * TMP_BIG_HEIGHT_MAP_SCALE;
			float tmpz = z * TMP_BIG_HEIGHT_MAP_SCALE;
			//tmp = getValue((float)x * TMP_BIG_HEIGHT_MAP_SCALE, (float)z * TMP_BIG_HEIGHT_MAP_SCALE);
			for (int layer = 0; layer < maxLayer; layer++)
			{
				tmp += ((1 + getValue(tmpx, tmpz)) * 0.5);
				tmpx *= 2;
				tmpz *= 2;
			}
			tmp /= (float)maxLayer;
		}
	}
	return *myBigHeightMap;// TODO Waw, nice leak bro
}

HeightMap	VoxelGenerator::createMap(float ox, float oz)
{
	ox *= (GRADIENT_SIZE / (float)MAX_NB_OF_CHUNK);
	oz *= (GRADIENT_SIZE / (float)MAX_NB_OF_CHUNK);
	HeightMap* myHeightMap = new HeightMap;
	int maxLayer = MAX_LAYER;
	for (int z = 0; z < HEIGHTMAP_SIZE; z++)
	{
		for (int x = 0; x < HEIGHTMAP_SIZE; x++)
		{	
			float* tmp = &(*myHeightMap)[z][x];
			float tmpz = oz + z * SCALE_HEIGHTMAP_TO_FRACTION_OF_NOISE;
			float tmpx = ox + x * SCALE_HEIGHTMAP_TO_FRACTION_OF_NOISE;

			//*tmp = (1 + getValue(tmpx, tmpz)) * 0.5f;
			//*tmp = *tmp > 0.3 ? *tmp : 0.3;
			
			//*tmp = getValue(ox + x * SCALE_HEIGHTMAP_TO_FRACTION_OF_NOISE, oz + z * SCALE_HEIGHTMAP_TO_FRACTION_OF_NOISE);
			float fractal = 0;
			for (int layer = 0; layer < maxLayer; layer++)
			{
				fractal += (1 + getValue(tmpx, tmpz)) * 0.5;
				tmpz *= 2;
				tmpx *= 2;
			}
			//*tmp = (*tmp + 1) / 2;
			*tmp = fractal;
			*tmp /= (float)maxLayer;		
		}
	}
	/*
	for (int z = 0; z < HEIGHTMAP_SIZE; z++)
	{
		for (int x = 0; x < HEIGHTMAP_SIZE; x++)
		{	
			(*myHeightMap)[z][x] /= maxLayer;
		}
	}
	*/
	return *myHeightMap;// TODO Waw, nice leak bro
}

// Function to linearly interpolate between a0 and a1
// Weight w should be in the range [0.0, 1.0]

float	VoxelGenerator::lerp(float a0, float a1, float w)
{
	return (1.0 - w)*a0 + w*a1;
}

void		VoxelGenerator::createPerlinGradient(unsigned long seed)
{
	std::mt19937 generator(seed);
	std::uniform_real_distribution<> distribution(0.f, 1.f);
	auto dice = [&generator, &distribution](){return distribution(generator);};
	float gradientLength;
	Gradients* newGradients = new Gradients;
	for (int i = 0; i < GRADIENT_SIZE; i++)
	{
		for (int j = 0; j < GRADIENT_SIZE; j++)
		{
			gradientLength = 2;
			float& x = (*newGradients)[i][j][0];
			float& y = (*newGradients)[i][j][1];
			while (gradientLength > 1)
			{
				x = 2 * dice() - 1;
				y = 2 * dice() - 1;
				gradientLength = x * x + y * y;
			}
			gradientLength = sqrtf(gradientLength);
			x /= gradientLength;
			y /= gradientLength;
		}
	}
	gradients = newGradients;
}

// Computes the dot product of the distance and gradient vectors.
float VoxelGenerator::dotGridGradient(int ix, int iy, float x, float y)
{

	// Precomputed (or otherwise) gradient vectors at each grid node

	// Compute the distance vector
	float dx = x - (float)ix;
	float dy = y - (float)iy;
	iy &= 255;
	ix &= 255;
	// Compute the dot-product
	return (dx * (*gradients)[iy][ix][0] + dy * (*gradients)[iy][ix][1]);
}

// Compute Perlin noise at coordinates x, y
float	VoxelGenerator::getValue(float x, float y)
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
