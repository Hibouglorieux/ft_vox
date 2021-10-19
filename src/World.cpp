/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   World.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/07/21 18:11:30 by nathan            #+#    #+#             */
/*   Updated: 2021/10/18 18:06:50 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "World.hpp"
#include <array>
#include "RectangularCuboid.hpp"


World::World()
{
	shader = new Shader();
	texture = new Texture();
	std::vector<Object*> objects = {};
	std::vector<Chunk*> chunks = {};
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			Chunk* chnk = new Chunk(i * CHUNK_WIDTH, j * CHUNK_DEPTH);
			chunks.push_back(chnk);
		}
	}
}

World::~World()
{
	for (auto it : objects)
	{
		delete it;
	}
	objects.clear();
	delete shader;
	delete texture;
}

void World::render()
{
	glEnable(GL_DEPTH_TEST);

	Matrix viewMat = camera.getMatrix();
	for (Object* object : objects)
	{
		object->draw(&viewMat);
	}

	// TODO remove that block when chunk are added and rework shader/texture owner
	Vec3 renderPos;
	shader->use();
	Matrix precalMat = Object::getProjMat() * camera.getMatrix();
    glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "precalcMat"), 1, GL_TRUE, precalMat.exportForGL());
    glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "projection"), 1, GL_TRUE, Object::getProjMat().exportForGL());
    glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "view"), 1, GL_TRUE, camera.getMatrix().exportForGL());
	//RectangularCuboid::draw(renderPos, shader, texture);
	Matrix	*del = new Matrix();
	for (Chunk* chnk : chunks)
	{
		std::cout << "Chunks\n";
		chnk->draw(del, renderPos, shader, texture);
	}
	Chunk* chnk = new Chunk(-1 * CHUNK_WIDTH, -1 * CHUNK_DEPTH);
	chnk->draw(del, renderPos, shader, texture);
	delete del;
	// TODO end of block to remove when chunk are added
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

void World::setTexture(Texture* newTexture)
{
	delete texture;
	texture = newTexture;
}

Camera& World::getCamera()
{
	return camera;
}
