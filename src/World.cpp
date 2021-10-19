/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   World.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/07/21 18:11:30 by nathan            #+#    #+#             */
/*   Updated: 2021/10/19 16:03:06 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "World.hpp"
#include <array>
#include "RectangularCuboid.hpp"


World::World()
{
	shader = new Shader();
	shader->use();
    glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "projection"), 1, GL_TRUE, Object::getProjMat().exportForGL());
	objects = {};
}

World::~World()
{
	for (auto it : objects)
	{
		delete it;
	}
	objects.clear();
	delete shader;
}

void World::render()
{
	Matrix precalculatedMat = Object::getProjMat() * camera.getMatrix();

    glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "precalcMat"), 1, GL_TRUE, precalculatedMat.exportForGL());
    glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "view"), 1, GL_TRUE, camera.getMatrix().exportForGL());
	for (Object* object : objects)
	{
		object->draw(shader);
	}
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
