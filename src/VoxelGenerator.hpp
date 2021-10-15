/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VoxelGenerator.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/15 13:08:43 by nathan            #+#    #+#             */
/*   Updated: 2021/10/15 13:11:58 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef VOXELGENERATION_CLASS_H
# define VOXELGENERATION_CLASS_H
#include <array>
#include "ft_vox.h"

typedef std::array<std::array<std::array<unsigned char, WIDTH>, LENGTH>, HEIGHT> VoxelMap;

class VoxelGenerator {
public:
	static VoxelMap	createMap(unsigned long seed);
private:
};

#endif
