/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VoxelGenerator.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/15 13:08:40 by nathan            #+#    #+#             */
/*   Updated: 2021/10/22 11:25:43 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "VoxelGenerator.hpp"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <random>

#define SCALE_HEIGHTMAP_TO_FRACTION_OF_NOISE ((float)1.0 / HEIGHTMAP_SIZE * GRADIENT_SIZE / (float)MAX_NB_OF_CHUNK)

#define MAX_LAYER 4
Gradients* VoxelGenerator::gradients = nullptr;
Gradients* VoxelGenerator::gradients2 = nullptr;
unsigned* VoxelGenerator::permutationTable = nullptr;
Gradient* VoxelGenerator::grad = nullptr;

void	VoxelGenerator::initialize(unsigned int seed)
{
	if (grad == nullptr || permutationTable == nullptr)
	{
		if (gradients != nullptr)
			delete gradients;
		gradients = createPerlinGradient(seed);
	}
	if (gradients == nullptr)
		gradients = createPerlinGradient(seed);
	if (gradients2 == nullptr)
		gradients2 = createPerlinGradient(0xFFFFFFFF - seed);
}

void	VoxelGenerator::clear()
{
	delete gradients;
}

HeightMap*	VoxelGenerator::createMap()
{
	HeightMap* myHeightMap = new HeightMap;
	for (int z = 0; z < HEIGHTMAP_SIZE; z++)
	{
		for (int x = 0; x < HEIGHTMAP_SIZE; x++)
		{
			(*myHeightMap)[z][x] = getValue((float)x * SCALE_HEIGHTMAP_TO_FRACTION_OF_NOISE, (float)z * SCALE_HEIGHTMAP_TO_FRACTION_OF_NOISE, gradients);
			float* tmp = &(*myHeightMap)[z][x];
			*tmp = (*tmp + 1) / 2;
		}
	}
	return myHeightMap;
}

BigHeightMap*	VoxelGenerator::createBigMap()
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
				tmp += ((1 + getValue(tmpx, tmpz, gradients)) * 0.5);
				tmpx *= 2;
				tmpz *= 2;
			}
			tmp /= (float)maxLayer;
		}
	}
	return myBigHeightMap;
}

BigHeightMap*	VoxelGenerator::createBigMap(int octaves = 1, float lacunarity = 2.0f, float gain = 0.5f)
{
	BigHeightMap* myBigHeightMap = new BigHeightMap;
	for (int z = 0; z < BIG_HEIGHT_MAP_SIZE; z++)
	{
		for (int x = 0; x < BIG_HEIGHT_MAP_SIZE; x++)
		{
#define TMP_BIG_HEIGHT_MAP_SCALE ((float)1.0 / (float)BIG_HEIGHT_MAP_SIZE * (float)GRADIENT_SIZE / 16.0)
			float& tmp = (*myBigHeightMap)[z][x];
			float tmpx = x * TMP_BIG_HEIGHT_MAP_SCALE;
			float tmpz = z * TMP_BIG_HEIGHT_MAP_SCALE;

			float amplitude = 1.0;
			float frequency = 1.0;
			float sum = 0.0;
			for(int i = 0; i < octaves; ++i)
			{
				sum += amplitude * (getValue(tmpx * frequency, tmpz * frequency
							, gradients));
				amplitude *= gain;
				frequency *= lacunarity;
			}
			tmp = sum;
		}
	}
	return myBigHeightMap;
}

HeightMap*	VoxelGenerator::createMap(float ox, float oz)
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
				fractal += (1 + getValue(tmpx, tmpz, gradients)) * 0.5;
				fractal += (1 + getValue(tmpx, tmpz, gradients2)) * 0.5;
				tmpz *= 2;
				tmpx *= 2;
			}
			//*tmp = (*tmp + 1) / 2;
			*tmp = fractal;
			*tmp /= ((float)maxLayer * 2);
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
	return myHeightMap;
}

HeightMap*	VoxelGenerator::createMap(float ox, float oz, int octaves = 1, float lacunarity = 2.0f, float gain = 0.5f)
{
	ox *= (GRADIENT_SIZE / (float)MAX_NB_OF_CHUNK);
	oz *= (GRADIENT_SIZE / (float)MAX_NB_OF_CHUNK);
	HeightMap* myHeightMap = new HeightMap;
	for (int z = 0; z < HEIGHTMAP_SIZE; z++)
	{
		for (int x = 0; x < HEIGHTMAP_SIZE; x++)
		{
			float* tmp = &(*myHeightMap)[z][x];
			float tmpz = oz + z * SCALE_HEIGHTMAP_TO_FRACTION_OF_NOISE;
			float tmpx = ox + x * SCALE_HEIGHTMAP_TO_FRACTION_OF_NOISE;

			float amplitude = 1.0;
			float frequency = 1.0;
			float sum = 0.0;
			for(int i = 0; i < octaves; ++i)
			{
				//sum += amplitude * (getValue(tmpx * frequency, tmpz * frequency
				//			, gradients));
				//sum += amplitude * (1.0 - fabsf(eval(tmpx * frequency, 0, tmpz * frequency)));
				sum += amplitude * eval(tmpx * frequency, 0, tmpz * frequency);
				amplitude *= gain;
				frequency *= lacunarity;
			}
			*tmp = fabsf(sum);
		}
	}
	return myHeightMap;
}

float	VoxelGenerator::getLocalDensity(float x, float y, float z,
		float ox, float oy, float oz,
		int octaves = 1, float lacunarity = 2.0f, float gain = 0.5f)
{
	ox *= (GRADIENT_SIZE / (float)MAX_NB_OF_CHUNK);
	oz *= (GRADIENT_SIZE / (float)MAX_NB_OF_CHUNK);
	oy *= (GRADIENT_SIZE / (float)MAX_NB_OF_CHUNK);
	float tmpz = oz + z * SCALE_HEIGHTMAP_TO_FRACTION_OF_NOISE;
	float tmpx = ox + x * SCALE_HEIGHTMAP_TO_FRACTION_OF_NOISE;
	float tmpy = oy + y * SCALE_HEIGHTMAP_TO_FRACTION_OF_NOISE;

	float amplitude = 1.0;
	float frequency = 1.0;
	float sum = 0.0;
	for(int i = 0; i < octaves; ++i)
	{
		sum += amplitude * eval(tmpx * frequency, tmpy * frequency, tmpz * frequency);
		amplitude *= gain;
		frequency *= lacunarity;
	}
	return sum;
}

// Function to linearly interpolate between a0 and a1
// Weight w should be in the range [0.0, 1.0]

float	VoxelGenerator::lerp(float a0, float a1, float w)
{
	return (1.0 - w) * a0 + w * a1;
}

float	VoxelGenerator::smoothstep(float t)
{
	return t * t * (3 - 2 * t);
}

float	VoxelGenerator::quintic(float t)
{
	return t * t * t * (t * (t * 6 - 15) + 10);
	// t^5 * 6 - t^4 * 15 + t^3 * 10
}

float	VoxelGenerator::quinticDeriv(float t)
{
	return 30 * t * t * (t * (t - 2) + 1);
	// (t^4 - t^3 * 2 + t^2) * 30
}

float	VoxelGenerator::seventic(float t)
{
	return t * t * t * t * (35 + (t * -84 + t * (t * 70 + t * t * -20)));
}

Gradients*	VoxelGenerator::createPerlinGradient(unsigned int seed)
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

	if (permutationTable == nullptr)
	{
		Gradient *gradi = new Gradient;
		unsigned* table = new unsigned[GRADIENT_SIZE * 2];
		for (int i = 0; i < GRADIENT_SIZE; i++)
		{
			gradientLength = 3;
			while (gradientLength > 1)
			{
				(*gradi)[i] = Vec3(2 * dice() - 1, 2 * dice() - 1, 2 - dice() - 1);
				gradientLength = (*gradi)[i].getSquaredLength();
			}
			(*gradi)[i] /= sqrtf(gradientLength);
			table[i] = i;
		}

		std::uniform_int_distribution<> distributionInt(0, GRADIENT_SIZE);
		auto diceInt = [&generator, &distributionInt](){return distributionInt(generator);};
		for (unsigned i = 0; i < GRADIENT_SIZE; i++)
			std::swap(table[i], table[diceInt() & (GRADIENT_SIZE - 1)]);
		for (unsigned i = 0; i < GRADIENT_SIZE; i++)
			table[GRADIENT_SIZE + i] = table[i];
		permutationTable = table;
		grad = gradi;
	}
	return newGradients;
}

// Computes the dot product of the distance and gradient vectors.
float VoxelGenerator::dotGridGradient(int ix, int iy, float x, float y, Gradients* grad)
{

	// Precomputed (or otherwise) gradient vectors at each grid node

	// Compute the distance vector
	float dx = x - (float)ix;
	float dy = y - (float)iy;
	iy &= GRADIENT_SIZE - 1;
	ix &= GRADIENT_SIZE - 1;
	// Compute the dot-product
	return (dx * (*grad)[iy][ix][0] + dy * (*grad)[iy][ix][1]);
}

// Computes the dot product of the distance and gradient vectors.
float VoxelGenerator::dotGridGradient(int ix, int iy, int iz, float x,
		float y, float z, Gradients* grad)
{
	(void)z;
	// Precomputed (or otherwise) gradient vectors at each grid node

	// Compute the distance vector
	float dx = x - (float)ix;
	float dy = y - (float)iy;
	//float dz = z - (float)iz;
	//iy &= GRADIENT_SIZE - 1;
	ix &= GRADIENT_SIZE - 1;
	iz &= GRADIENT_SIZE - 1;
	// Compute the dot-product
	return (dx * (*grad)[iz][ix][0] + dy * (*grad)[iz][ix][1]);
}


// Compute Perlin noise at coordinates x, y
float	VoxelGenerator::getValue(float x, float y, Gradients* grad)
{
	// Determine grid cell coordinates
	int x0 = (int)floor(x);
	int x1 = x0 + 1;
	int y0 = (int)floor(y);
	int y1 = y0 + 1;

	// Determine interpolation weights
	// Could also use higher order polynomial/s-curve here
	float sx = quintic(x - (float)x0);
	float sy = quintic(y - (float)y0);

	// Interpolate between grid point gradients
	float n0, n1, ix0, ix1, value;
	n0 = dotGridGradient(x0, y0, x, y, grad);
	n1 = dotGridGradient(x1, y0, x, y, grad);
	ix0 = lerp(n0, n1, sx);
	n0 = dotGridGradient(x0, y1, x, y, grad);
	n1 = dotGridGradient(x1, y1, x, y, grad);
	ix1 = lerp(n0, n1, sx);
	value = lerp(ix0, ix1, sy);

	return value;
}

int VoxelGenerator::hash(int &x, int &y, int &z)
{
	return permutationTable[permutationTable[permutationTable[x] + y] + z];
}

float	VoxelGenerator::gradientDotV(int perm, float x, float y, float z)
{
	switch(perm & 15)
	{
		case  0: return  x + y; // (1, 1, 0)
		case  1: return -x + y; // (-1, 1, 0)
		case  2: return  x - y; // (1, -1, 0)
		case  3: return -x - y; // (-1, -1, 0)
		case  4: return  x + z; // (1, 0, 1)
		case  5: return -x + z; // (-1, 0, 1)
		case  6: return  x - z; // (1, 0, -1)
		case  7: return -x - z; // (-1, 0, -1)
		case  8: return  y + z; // (0, 1, 1)
		case  9: return -y + z; // (0, -1, 1)
		case 10: return  y - z; // (0, 1, -1)
		case 11: return -y - z; // (0, -1, -1)
		case 12: return  y + x; // (1, 1, 0)
		case 13: return -y + x; // (1, -1, 0)
		case 14: return -y + z; // (0, -1, 1)
		case 15: return -y - z; // (0, -1, -1)
	}
	return 0.0;
}

float VoxelGenerator::eval(float x, float y, float z)
{
	//x *= 0.25;
	//z *= 0.25;
	int xi0 = (int)floor(x) & (GRADIENT_SIZE - 1);
	int yi0 = (int)floor(y) & (GRADIENT_SIZE - 1);
	int zi0 = (int)floor(z) & (GRADIENT_SIZE - 1);

	int xi1 = (xi0 + 1) & (GRADIENT_SIZE - 1);
	int yi1 = (yi0 + 1) & (GRADIENT_SIZE - 1);
	int zi1 = (zi0 + 1) & (GRADIENT_SIZE - 1);

	float tx = x - (float)floor(x);
	float ty = y - (float)floor(y);
	float tz = z - (float)floor(z);

	float u = quintic(tx);
	float v = quintic(ty);
	float w = quintic(tz);
	/*float u = smoothstep(tx);
	float v = smoothstep(ty);
	float w = smoothstep(tz);*/

	float x0 = tx;
	float x1 = tx - 1;
	float y0 = ty;
	float y1 = ty - 1;
	float z0 = tz;
	float z1 = tz - 1;

	float a = gradientDotV(hash(xi0, yi0, zi0), x0, y0, z0);
	float b = gradientDotV(hash(xi1, yi0, zi0), x1, y0, z0);
	float c = gradientDotV(hash(xi0, yi1, zi0), x0, y1, z0);
	float d = gradientDotV(hash(xi1, yi1, zi0), x1, y1, z0);
	float e = gradientDotV(hash(xi0, yi0, zi1), x0, y0, z1);
	float f = gradientDotV(hash(xi1, yi0, zi1), x1, y0, z1);
	float g = gradientDotV(hash(xi0, yi1, zi1), x0, y1, z1);
	float h = gradientDotV(hash(xi1, yi1, zi1), x1, y1, z1);

	float k0 = a;
	float k1 = (b - a);
	float k2 = (c - a);
	float k3 = (e - a);
	float k4 = (a + d - b - c);
	float k5 = (a + f - b - e);
	float k6 = (a + g - c - e);
	float k7 = (b + c + e + h - a - d - f - g);

	return k0 +
		k1 * u +
		k2 * v +
		k3 * w +
		k4 * u * v +
		k5 * u * w +
		k6 * v * w +
		k7 * u * v * w;
}
