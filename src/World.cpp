/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   World.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/07/21 18:11:30 by nathan            #+#    #+#             */
/*   Updated: 2021/10/18 12:32:53 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "World.hpp"
#include <array>

World::World()
{
	std::vector<Object*> objects = {};
}

World::~World()
{
	for (auto it : objects)
	{
		delete it;
	}
	objects.clear();
}

void World::render()
{
	glEnable(GL_DEPTH_TEST);

	Matrix viewMat = camera.getMatrix();
	for (Object* object : objects)
	{
		object->draw(&viewMat);
	}
	glDisable(GL_DEPTH_TEST);
}


void World::addObject(Object* newobj)
{
	objects.push_back(newobj);
}

std::vector<Object*>& World::getObjects()
{
	return objects;
}

void World::setCamera(Camera newCamera)
{
	camera= newCamera;
}

Camera& World::getCamera()
{
	return camera;
}
