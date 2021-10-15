/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VoxelGenerator.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/15 13:08:40 by nathan            #+#    #+#             */
/*   Updated: 2021/10/15 13:11:36 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "VoxelGenerator.hpp"

VoxelMap	VoxelGenerator::createMap(unsigned long seed)
{
	(void)seed;
	VoxelMap* pouet = new VoxelMap;
	return *pouet;
}
