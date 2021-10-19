/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VoxelGenerator.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/15 13:08:43 by nathan            #+#    #+#             */
/*   Updated: 2021/10/19 17:44:21 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef VOXELGENERATION_CLASS_H
# define VOXELGENERATION_CLASS_H
#include <array>
#include "ft_vox.h"

#define GRADIENT_SIZE 32
#define HEIGHTMAP_SIZE 128

typedef std::array<std::array<std::array<unsigned char, WIDTH>, LENGTH>, HEIGHT> VoxelMap;
typedef std::array<std::array<float, HEIGHTMAP_SIZE>, HEIGHTMAP_SIZE> HeightMap;

class VoxelGenerator {
public:
	static HeightMap	createMap(unsigned long seed);
	static HeightMap	createMap(unsigned long seed, int ox, int oz);
private:
};

class PerlinNoise {
	public:
		PerlinNoise(unsigned long seed);
		float getValue(float x, float y);
	private:
		std::array<std::array<std::array<float, 2>, GRADIENT_SIZE>, GRADIENT_SIZE> gradients;
		float dotGridGradient(int ix, int iy, float x, float y);
		float lerp(float a0, float a1, float w) const ;

};

#endif
