/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   World.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/07/21 18:11:30 by nathan            #+#    #+#             */
/*   Updated: 2021/10/15 13:04:00 by nathan           ###   ########.fr       */
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
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);

		glEnable(GL_DEPTH_TEST);


	Matrix viewMat = camera.getMatrix();
	for (Object* object : objects)
	{
		object->draw(&viewMat);
	}

		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glDisable(GL_DEPTH_TEST);

	std::array<float, 3> color = {0.0f, 0.0f, 0.0f};
	Shader specialEffect("new.vert", "outline.frag");
	std::vector<std::tuple<std::function<void(GLint, GLsizei, const GLfloat*)>, std::string, const GLfloat*>> shaderData;
	std::tuple<std::function<void(GLint, GLsizei, const GLfloat*)>, std::string, const GLfloat*> functionCall = std::make_tuple(glUniform3fv, "myColor", &color.front());
	shaderData.push_back(functionCall);
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

void World::clearObject(std::string ID)
{
	for (Object*& obj : objects)
	{
		if (obj->getID() == ID)
		{
			delete obj;
			obj = nullptr;
		}
	}
	objects.erase(std::remove(objects.begin(), objects.end(), nullptr), objects.end());
}
