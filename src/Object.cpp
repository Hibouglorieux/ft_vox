/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Object.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/08 19:05:07 by nathan            #+#    #+#             */
/*   Updated: 2021/10/18 12:21:39 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Object.hpp"
#include "ft_vox.h"

Matrix Object::projMat = Matrix::createProjMatrix(FOV, SCREEN_WIDTH / SCREEN_HEIGHT, NEAR, FAR);


Object::Object( void )
{
}

Object::~Object( void )
{
}

void Object::setProjMat(Matrix newProjMat)
{
	projMat = newProjMat;	
}
