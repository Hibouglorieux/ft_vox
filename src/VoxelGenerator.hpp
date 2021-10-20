/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VoxelGenerator.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/15 13:08:43 by nathan            #+#    #+#             */
/*   Updated: 2021/10/20 15:43:44 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef VOXELGENERATION_CLASS_H
# define VOXELGENERATION_CLASS_H
#include <array>
#include "ft_vox.h"

#define GRADIENT_SIZE 256 // will change in the future, it is 2pow(n) + 1 because
						// it's not yet looping on itself
						// bigger == bigger noise
						// is currently stretched to all chunk displayed as testing
#define HEIGHTMAP_SIZE BLOC_WIDTH_PER_CHUNK

typedef std::array<std::array<std::array<unsigned char, WIDTH>, LENGTH>, HEIGHT> VoxelMap;
typedef std::array<std::array<float, HEIGHTMAP_SIZE>, HEIGHTMAP_SIZE> HeightMap;
typedef std::array<std::array<std::array<float, 2>, GRADIENT_SIZE>, GRADIENT_SIZE> Gradients;

class VoxelGenerator {
public:
	static void			Initialize(unsigned long seed);
	static HeightMap	createMap();
	static HeightMap	createMap(float ox, float oz);
private:
	static void	createPerlinGradient(unsigned long seed);
	static float dotGridGradient(int ix, int iy, float x, float y);
	static float getValue(float x, float y);
	static float lerp(float a0, float a1, float w);

	static Gradients* gradients;
};


#endif
