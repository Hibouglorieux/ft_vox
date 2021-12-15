/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VoxelGenerator.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/15 13:08:40 by nathan            #+#    #+#             */
/*   Updated: 2021/12/10 16:56:03 by nallani          ###   ########.fr       */
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
WorleyGradient* VoxelGenerator::worleyGradient = nullptr;

static const uint8_t perm[256] = {
    151, 160, 137, 91, 90, 15,
    131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23,
    190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
    88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166,
    77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244,
    102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196,
    135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123,
    5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42,
    223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9,
    129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228,
    251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107,
    49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
    138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180
};

static const Vec3 defaultGrad[16] = {
	Vec3(1, 1, 0), Vec3(-1, 1, 0), Vec3(1, -1, 0), Vec3(-1, -1, 0),
	Vec3(1, 0, 1), Vec3(-1, 0, 1), Vec3(1, 0, -1), Vec3(-1, 0, -1),
	Vec3(0, 1, 1), Vec3(0, -1, 1), Vec3(0, 1, -1), Vec3(0, -1, -1),
	Vec3(1, 1, 0), Vec3(-1, 1, 0), Vec3(1, -1, 0), Vec3(-1, -1, 0)
};

permTable 	VoxelGenerator::permsP;
gradTable 	VoxelGenerator::gradsP;
bool		VoxelGenerator::tableInit = false;

void	VoxelGenerator::initializePermutations(int seed, int tableId)
{
	seed *= 79864;

	for (int i = 0; i < 256; i++)
	{
		int value = perm[i] ^ (seed & 255);	// Maybe try to add more variation
		if ((i & 1) != 1)
			value = perm[i] ^ ((seed >> 8) & 255);
		
		permsP[tableId][i] = value;
		permsP[tableId][i + 256] = value;
		gradsP[tableId][i] = defaultGrad[value % 16];
		gradsP[tableId][i + 256] = defaultGrad[value % 16];
	}
}

void	VoxelGenerator::shuffleTables(int seed, int tableId) // Fisher-Yates
{
	std::mt19937 generator(seed);
	std::uniform_real_distribution<> distribution(0.f, 1.f);
	auto dice = [&generator, &distribution](){return distribution(generator);};

	for (int i = permsP[tableId].size() - 1; i > 0; i--)
	{
		int j = (int)dice() * (i - 0.0001);
		int a = permsP[tableId][i];
		int b = permsP[tableId][j];

		permsP[tableId][i] = b;
		permsP[tableId][j] = a;
	}
}

void 	VoxelGenerator::initialize(int seed, bool toDelete)
{
	(void)toDelete;

	VoxelGenerator::initializePermutations(seed, 0);
	VoxelGenerator::initializePermutations(seed * 1.62, 1);
	VoxelGenerator::initializePermutations(seed * 3.24, 2);

	
	VoxelGenerator::shuffleTables(seed * 1.62, 0);
	VoxelGenerator::shuffleTables(seed * 3.24, 1);
	VoxelGenerator::shuffleTables(seed * 6.48, 2);

	VoxelGenerator::tableInit = true;
}

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
	if (worleyGradient == nullptr)
		worleyGradient = createWorleyGradient(seed);
}

WorleyGradient* VoxelGenerator::createWorleyGradient(unsigned int seed)
{
	std::mt19937 generator(seed);
	std::uniform_real_distribution<> distribution(0.f, 1.f);
	auto dice = [&generator, &distribution](){return distribution(generator);};
	WorleyGradient* worley = new WorleyGradient;
	for (int i = 0; i < WORLEY_SIZE; i++)
		for (int j = 0; j < WORLEY_SIZE; j++)
			for (int k = 0; k < WORLEY_SIZE; k++)
			{
				float& x = (*worley)[k][i][j][0];
				float& y = (*worley)[k][i][j][1];
				float& z = (*worley)[k][i][j][2];
				x = dice() - 0.5f;
				y = dice() - 0.5f;
				z = dice() - 0.5f;
			}
	/*
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			for (int k = 0; k < 3; k++)
			{
				(*worley)[i][j][k].print();
			}
			*/
	return worley;
}

float	VoxelGenerator::getWorleyValueAt(float x, float y, float z)
{
	x = fmod((fmod(x, WORLEY_SIZE) + WORLEY_SIZE), WORLEY_SIZE);
	y = fmod((fmod(y, WORLEY_SIZE) + WORLEY_SIZE), WORLEY_SIZE);
	z = fmod((fmod(z, WORLEY_SIZE) + WORLEY_SIZE), WORLEY_SIZE);
	float	unused;
	Vec3 curPos(std::modf(x, &unused) - 0.5, std::modf(y, &unused) - 0.5, std::modf(z, &unused) - 0.5);// where is that point in the cube
	float min = 100.f;
	for (int yy = y -1; yy < y + 2; yy++)
		for (int zz = z -1; zz < z + 2; zz++)
			for (int xx = x -1; xx < x + 2; xx++)
			{
				int xClamp = (xx % WORLEY_SIZE + WORLEY_SIZE) % WORLEY_SIZE;
				int yClamp = (yy % WORLEY_SIZE + WORLEY_SIZE) % WORLEY_SIZE;
				int zClamp = (zz % WORLEY_SIZE + WORLEY_SIZE) % WORLEY_SIZE;
				Vec3 worleyPos((*worleyGradient)[xClamp][yClamp][zClamp] + (Vec3(xx - (int)x, yy - (int)y, zz - (int)z))); //
				float distance = (worleyPos - curPos).getLength();
				if (min > distance)
					min = distance;
			}
	return min;
}

void	VoxelGenerator::clear()
{
	delete gradients;
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
				sum += amplitude * eval(tmpx * frequency, 0, tmpz * frequency, 2);
				amplitude *= gain;
				frequency *= lacunarity;
			}
			*tmp = (sum + 1) * 0.5;
			if (*tmp < 0)
				*tmp = 0;
			else if (*tmp >= 1.0)
				*tmp = 1;
		}
	}
	return myHeightMap;
}

CaveMap*	VoxelGenerator::createCaveMap(float ox, float oz, int octaves = 1, float lacunarity = 2.0f, float gain = 0.5f)
{
	ox *= (GRADIENT_SIZE / (float)MAX_NB_OF_CHUNK);
	oz *= (GRADIENT_SIZE / (float)MAX_NB_OF_CHUNK);
	CaveMap* cavemap = new CaveMap;
	for (int y = 0; y < 64; y++)
		for (int z = 0; z < HEIGHTMAP_SIZE; z++)
			for (int x = 0; x < HEIGHTMAP_SIZE; x++)
			{
				float* tmp = &(*cavemap)[y][z][x];
				float tmpz = oz + z * SCALE_HEIGHTMAP_TO_FRACTION_OF_NOISE;
				float tmpx = ox + x * SCALE_HEIGHTMAP_TO_FRACTION_OF_NOISE;
				float tmpy = y / 64.0f;

				float amplitude = 0.05;
				float frequency = .5;
				float sum = 0.0;
				for(int i = 0; i < octaves; ++i)
				{
					//sum += amplitude * (getValue(tmpx * frequency, tmpz * frequency
					//			, gradients));
					//sum += amplitude * (1.0 - fabsf(eval(tmpx * frequency, 0, tmpz * frequency)));
					sum += amplitude * eval(tmpx * frequency, tmpy * frequency, tmpz * frequency, 2);
					amplitude *= gain;
					frequency *= lacunarity;
				}
				*tmp = fabsf(sum);
				//std::cout << " " << *tmp << std::endl;
			}
	return cavemap;
}

// Function to linearly interpolate between a0 and a1
// Weight w should be in the range [0.0, 1.0]

float	VoxelGenerator::lerp(float a0, float a1, float w)
{
	return std::fma((1.0 - w), a0, w * a1);
}

float	VoxelGenerator::smoothstep(float t)
{
	return t * t * (3 - 2 * t);
}

float	VoxelGenerator::quintic(float t)
{
	return t * t * t * (t * (t * 6 - 15) + 10);// need explanation
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

inline int VoxelGenerator::hash(int &x, int &y, int &z, int tableId)
{
	(void)tableId;
	return permsP[tableId][permsP[tableId][permsP[tableId][x] + y] + z];
	//return permutationTable[permutationTable[permutationTable[x] + y] + z];
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

static inline int32_t fastfloor(float fp) {
    int32_t i = static_cast<int32_t>(fp);
    return (fp < i) ? (i - 1) : (i);
}

float VoxelGenerator::eval(float x, float y, float z, int tableId)
{
	/*x *= 0.25;
	z *= 0.25;*/

	x *= 7.5;
	z *= 7.5;

	int xi0 = (int)fastfloor(x) & (GRADIENT_SIZE - 1);
	int yi0 = (int)fastfloor(y) & (GRADIENT_SIZE - 1);
	int zi0 = (int)fastfloor(z) & (GRADIENT_SIZE - 1);

	int xi1 = (xi0 + 1) & (GRADIENT_SIZE - 1);
	int yi1 = (yi0 + 1) & (GRADIENT_SIZE - 1);
	int zi1 = (zi0 + 1) & (GRADIENT_SIZE - 1);

	float tx = x - (float)fastfloor(x);
	float ty = y - (float)fastfloor(y);
	float tz = z - (float)fastfloor(z);

	/*float u = quintic(tx);
	float v = quintic(ty);
	float w = quintic(tz);*/
	float u = smoothstep(tx);
	float v = smoothstep(ty);
	float w = smoothstep(tz);

	float x0 = tx;
	float x1 = tx - 1;
	float y0 = ty;
	float y1 = ty - 1;
	float z0 = tz;
	float z1 = tz - 1;

	float a = gradientDotV(hash(xi0, yi0, zi0, tableId), x0, y0, z0);
	float b = gradientDotV(hash(xi1, yi0, zi0, tableId), x1, y0, z0);
	float c = gradientDotV(hash(xi0, yi1, zi0, tableId), x0, y1, z0);
	float d = gradientDotV(hash(xi1, yi1, zi0, tableId), x1, y1, z0);
	float e = gradientDotV(hash(xi0, yi0, zi1, tableId), x0, y0, z1);
	float f = gradientDotV(hash(xi1, yi0, zi1, tableId), x1, y0, z1);
	float g = gradientDotV(hash(xi0, yi1, zi1, tableId), x0, y1, z1);
	float h = gradientDotV(hash(xi1, yi1, zi1, tableId), x1, y1, z1);

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

float	VoxelGenerator::Noise2D(float x, float z, float output, float frequency, float amplitude, int octaves, int tableId, float lacunarity, float gain)
{
	for(int i = 0; i < octaves; ++i)
	{
		output += amplitude * eval(x * frequency, 0 * frequency, z * frequency, tableId);

		amplitude *= gain;
		frequency *= lacunarity;
	}
	output = (output + 1) * 0.5;
	if (output < 0)
		output = 0;
	else if (output >= 1.0)
		output = 1;
	return output;
}

float	VoxelGenerator::Noise3D(float x, float y, float z, float output, float frequency, float amplitude, int octaves, int tableId, float lacunarity, float gain)
{
	for(int i = 0; i < octaves; ++i)
	{
		output += amplitude * eval(x * frequency, y * frequency, z * frequency, tableId);

		amplitude *= gain;
		frequency *= lacunarity;
	}
	output = (output + 1) * 0.5;

	// We to cleave/clamp the cavern noise;
	output = pow(output, 1.65);
	if (output < 0)
		output = 0;
	else if (output >= 1.0)
		output = 1;
	return output;
}

/*inline uint8_t hash(int32_t i, int tableId) {
    return permsP[tableId][static_cast<uint8_t>(i)];
}*/

/**
 * 2D Perlin simplex noise
 *
 *  Takes around 150ns on an AMD APU.
 *
 * @param[in] x float coordinate
 * @param[in] y float coordinate
 *
 * @return Noise value in the range[-1; 1], value of 0 on all integer coordinates.
 */
float VoxelGenerator::simplexNoise2D(float x, float y, int tableId) {
    float n0, n1, n2;   // Noise contributions from the three corners

    // Skewing/Unskewing factors for 2D
    static const float F2 = 0.366025403f;  // F2 = (sqrt(3) - 1) / 2
    static const float G2 = 0.211324865f;  // G2 = (3 - sqrt(3)) / 6   = F2 / (1 + 2 * K)

    // Skew the input space to determine which simplex cell we're in
    const float s = (x + y) * F2;  // Hairy factor for 2D
    const float xs = x + s;
    const float ys = y + s;
    int32_t i = fastfloor(xs);
    int32_t j = fastfloor(ys);

    // Unskew the cell origin back to (x,y) space
    const float t = static_cast<float>(i + j) * G2;
    const float X0 = i - t;
    const float Y0 = j - t;
    const float x0 = x - X0;  // The x,y distances from the cell origin
    const float y0 = y - Y0;

    // For the 2D case, the simplex shape is an equilateral triangle.
    // Determine which simplex we are in.
    int32_t i1, j1;  // Offsets for second (middle) corner of simplex in (i,j) coords
    if (x0 > y0)
	{   // lower triangle, XY order: (0,0)->(1,0)->(1,1)
        i1 = 1;
        j1 = 0;
    }
	else
	{   // upper triangle, YX order: (0,0)->(0,1)->(1,1)
        i1 = 0;
        j1 = 1;
    }

    // A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
    // a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
    // c = (3-sqrt(3))/6

    const float x1 = x0 - i1 + G2;            // Offsets for middle corner in (x,y) unskewed coords
    const float y1 = y0 - j1 + G2;
    const float x2 = x0 - 1.0f + 2.0f * G2;   // Offsets for last corner in (x,y) unskewed coords
    const float y2 = y0 - 1.0f + 2.0f * G2;

    // Work out the hashed gradient indices of the three simplex corners
    /*const int gi0 = hash(i + hash(j));
    const int gi1 = hash(i + i1 + hash(j + j1));
    const int gi2 = hash(i + 1 + hash(j + 1));*/

	i = i % 255;
	j = j % 255;
    // Calculate the contribution from the first corner
	Vec3 g;
    float t0 = 0.5f - x0*x0 - y0*y0;
    if (t0 < 0.0f) {
        n0 = 0.0f;
    } else {
        t0 *= t0;
        //n0 = t0 * t0 * grad(gi0, x0, y0);
        g = gradsP[tableId][i + permsP[tableId][j]];
        n0 = t0 * t0 * (g.x * x0 + g.y * y0);
    }

    // Calculate the contribution from the second corner
    float t1 = 0.5f - x1*x1 - y1*y1;
    if (t1 < 0.0f) {
        n1 = 0.0f;
    } else {
        t1 *= t1;
        //n1 = t1 * t1 * grad(gi1, x1, y1);
        g = gradsP[tableId][i + (int)i1 + permsP[tableId][j + (int)j1]];
        n1 = t1 * t1 * (g.x * x1 + g.y * y1);
    }

    // Calculate the contribution from the third corner
    float t2 = 0.5f - x2*x2 - y2*y2;
    if (t2 < 0.0f) {
        n2 = 0.0f;
    } else {
        t2 *= t2;
        //n2 = t2 * t2 * grad(gi2, x2, y2);
        g = gradsP[tableId][i + 1 + permsP[tableId][j + 1]];
        n2 = t2 * t2 * (g.x * x2 + g.y * y2);
    }

    // Add contributions from each corner to get the final noise value.
    // The result is scaled to return values in the interval [-1,1].
    return 45.23065f * (n0 + n1 + n2);
}
