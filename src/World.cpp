/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   World.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/07/21 18:11:30 by nathan            #+#    #+#             */
/*   Updated: 2021/11/30 19:30:25 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "World.hpp"
#include <array>
#include "RectangularCuboid.hpp"
#include <chrono>
#include <unistd.h>

#define WAITING_THREAD_TIME (0.1 * SEC_TO_MICROSEC)

World::World()
{
	shader = new Shader();
	shader->use();
	glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "projection"), 1, GL_TRUE, Object::getProjMat().exportForGL());
	visibleChunks = {};
	preLoadedChunks = {};
	curPos = Chunk::worldCoordToChunk(camera.getPos());

	Skybox::initialize();
	auto positions = getPosInRange(Vec2(), 0, CHUNK_VIEW_DISTANCE);
	for (auto pos : positions)
	{
			auto allocatedNeighbours = getAllocatedNeighbours(pos);
			for (auto it: allocatedNeighbours)
				it.second->increaseThreadCount();
			Chunk* chnk = new Chunk(pos.x, pos.y, allocatedNeighbours);
			visibleChunks.insert(std::pair<Vec2, Chunk*>(pos, chnk));

			auto initNewChunk = [](Chunk *chnk) {
				chnk->initChunk();
			};

			std::thread worker(initNewChunk, chnk);
			worker.detach();
	}
	positions.clear();
	positions = getPosInRange(Vec2(), CHUNK_VIEW_DISTANCE, MAX_PRELOAD_DISTANCE);
	for (auto pos : positions)
	{
		if (visibleChunks.find(pos) == visibleChunks.end())
		{
			auto allocatedNeighbours = getAllocatedNeighbours(pos);
			for (auto it: allocatedNeighbours)
				it.second->increaseThreadCount();
			Chunk* chnk = new Chunk(pos.x, pos.y, allocatedNeighbours);
			preLoadedChunks.insert(std::pair<Vec2, Chunk*>(pos, chnk));

			auto initNewChunk = [](Chunk *chnk) {
				chnk->initChunk();
			};

			std::thread worker(initNewChunk, chnk);
			worker.detach();
		}
	}
	/*
	for (int i = -ROW_OF_CHUNK; i < ROW_OF_CHUNK; i++) // defined in VoxelGenerator, needed to scale the heightmap calculation
	{
		for (int j = -ROW_OF_CHUNK; j < ROW_OF_CHUNK; j++)
		{
			Chunk* chnk = new Chunk(j, i);
			//addObject(chnk);
			visibleChunks.insert(std::pair<Vec2, Chunk*>(Vec2(j, i), chnk));

			auto initNewChunk = [](Chunk *chnk) {
				chnk->initChunk();
			};

			std::thread worker(initNewChunk, chnk);
			worker.detach();
		}
	}
	*/
}

World::~World()
{
	auto it = visibleChunks.begin();
	while (it != visibleChunks.end())
	{
		if (!(*it).second->hasThreadFinished())
		{
			usleep(WAITING_THREAD_TIME);
		}
		else
		{
			delete (*it).second;
			it++;
		}
	}
	it = preLoadedChunks.begin();
	while (it != preLoadedChunks.end())
	{
		if (!(*it).second->hasThreadFinished())
		{
			usleep(WAITING_THREAD_TIME);
		}
		else
		{
			delete (*it).second;
			it++;
		}
	}
	delete shader;
	Skybox::clear();
}

void World::render()
{
	//std::cout << camera.getPos().x << "," << camera.getPos().z << std::endl;

	Matrix precalculatedMat = Object::getProjMat() * camera.getMatrix();


	// draw skybox
	Skybox::draw(precalculatedMat);

	// draw chunks
	shader->use();
	glEnable(GL_DEPTH_TEST);
	glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "precalcMat"), 1, GL_TRUE, precalculatedMat.exportForGL());
	glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "view"), 1, GL_TRUE, camera.getMatrix().exportForGL());

	glUniform1i(glGetUniformLocation(shader->getID(), "water"), BLOCK_WATER);
	glUniform1i(glGetUniformLocation(shader->getID(), "sand"), BLOCK_SAND);
	glUniform1i(glGetUniformLocation(shader->getID(), "dirt"), BLOCK_DIRT);
	glUniform1i(glGetUniformLocation(shader->getID(), "grass"), BLOCK_GRASS);
	glUniform1i(glGetUniformLocation(shader->getID(), "grass_snow"), BLOCK_GRASS_SNOW);
	glUniform1i(glGetUniformLocation(shader->getID(), "stone"), BLOCK_STONE);
	glUniform1i(glGetUniformLocation(shader->getID(), "snow"), BLOCK_SNOW);
	glUniform1i(glGetUniformLocation(shader->getID(), "bedrock"), BLOCK_BEDROCK);

	ResourceManager::bindTextures();
	int renderCount = 0;
	for (auto it : visibleChunks)
	{
		Chunk*& chnk = it.second;
		if (shouldBeRendered(it.first, chnk, precalculatedMat))
		{
			chnk->draw(shader);
			renderCount++;
		}
	}
	//std::cout << "rendered: " << renderCount << "on total visible chunk: " << visibleChunks.size() << std::endl;
}

void World::update()
{
	Vec2 newChunkPos = Chunk::worldCoordToChunk(camera.getPos());
	if (curPos != newChunkPos)
	{
		curPos = newChunkPos;
		updateChunkBuffers(curPos);
	}
}

void World::updateChunkBuffers(Vec2 newPos)
{
	std::vector<Vec2> posBuffer = {};


	//delete useless chunk
	for (auto it : preLoadedChunks)
	{
		Vec2 pos = it.first;
		Vec2 relativePos = pos - newPos;
		Chunk*& chunk = it.second;
		if (relativePos.getLength() >= PRELOAD_DISTANCE_DEL && chunk->hasThreadFinished())
		{
			delete chunk;
			posBuffer.push_back(pos);
		}
	}
	for (Vec2& pos: posBuffer)
		preLoadedChunks.erase(pos);
	
	//remove old visible to preloaded
	posBuffer.clear();
	for (auto it : visibleChunks)
	{
		const Vec2& pos = it.first;
		Vec2 relativePos = pos - newPos;
		//relativePos.print();
		//std::cout << relativePos.getLength() << std::endl;
		Chunk*& chunk = it.second;
		if (relativePos.getLength() >= CHUNK_VIEW_DISTANCE)
		{
			if (preLoadedChunks.find(pos) != preLoadedChunks.end())
			{
				std::cout << "yooo wtf" << std::endl;
				pos.print();
			}
			preLoadedChunks.insert(std::pair<Vec2, Chunk*>(pos, chunk));
			posBuffer.push_back(pos);
		}
	}
	for (Vec2& pos: posBuffer)
	{
		visibleChunks.erase(pos);
	}

	//preload new chunk
	
	posBuffer.clear();
	posBuffer = getPosInRange(newPos, CHUNK_VIEW_DISTANCE, MAX_PRELOAD_DISTANCE);
	auto initNewChunks = [](std::vector<Chunk*> chunks) { for (auto it : chunks) it->initChunk();};
	std::vector<Chunk*> chunks;
	for (Vec2& preloadedPositions : posBuffer)
	{
		if (preLoadedChunks.find(preloadedPositions) == preLoadedChunks.end())
		{
			auto allocatedNeighbours = getAllocatedNeighbours(preloadedPositions);
			for (auto it: allocatedNeighbours)
				it.second->increaseThreadCount();
			chunks.push_back(new Chunk(preloadedPositions.x, preloadedPositions.y, allocatedNeighbours));
			preLoadedChunks.insert(std::pair<Vec2, Chunk*>(preloadedPositions, chunks.back()));
		}
	}
	std::thread worker(initNewChunks, chunks);
	worker.detach();
		
	
	//move from preloaded to visible // or load if thats not the case ?
	posBuffer = getPosInRange(newPos, 0, CHUNK_VIEW_DISTANCE);
	for (auto key : posBuffer)
	{
		if (visibleChunks.find(key) == visibleChunks.end())
		{
			if (preLoadedChunks.find(key) == preLoadedChunks.end())
				std::cerr << "SHOULDNT HAPPEN, tried to moved from preloaded to visible but it wasnt preloaded" << std::endl;
			else 
			{
				std::pair<Vec2, Chunk*> elem(key, preLoadedChunks[key]);
				preLoadedChunks.erase(key);
				visibleChunks.insert(elem);
			}
		}
	}
}

std::vector<Vec2> World::getPosInRange(Vec2 center, float minDistance, float maxDistance)
{
	std::vector<Vec2> Positions;
	for (int y = -maxDistance; y < maxDistance; y++)
		for (int x = -maxDistance; x < maxDistance; x++)
		{
			Vec2 relative(x, y);
			if (relative.getLength() >= minDistance && relative.getLength() < maxDistance)
			{
				Vec2 absolute = relative + center;
				if (absolute.x > MAX_NB_OF_CHUNK / -2 && absolute.y > MAX_NB_OF_CHUNK / -2 
						&& absolute.x < MAX_NB_OF_CHUNK / 2 && absolute.y < MAX_NB_OF_CHUNK / 2)
					Positions.push_back(absolute);
			}
		}
	return Positions;
}

std::vector<std::pair<Vec2, Chunk*>>	World::getAllocatedNeighbours(Vec2 chunkPos)
{
	std::vector<std::pair<Vec2, Chunk*>> neighbours;
	for (int y = -1; y < 2; y++)
		for (int x = -1; x < 2; x++)
		{
			if ((y == 0) != (x == 0))
			{
				Vec2 relativePos = chunkPos + Vec2(x, y);
				auto searchResult = visibleChunks.find(relativePos);
				if (searchResult != visibleChunks.end())
					neighbours.push_back(*searchResult);
				else
				{
					searchResult = preLoadedChunks.find(relativePos);
					if (searchResult != preLoadedChunks.end())
						neighbours.push_back(*searchResult);
				}
			}
		}
	return neighbours;
}

bool	World::shouldBeRendered(Vec2 chunkPos, Chunk* chnk, Matrix& matrix)
{
	if (chunkPos == curPos)
		return true;
	Vec3 topLeft = chnk->getPos();
	Vec3 botRight = topLeft + Vec3(CHUNK_WIDTH, 0, CHUNK_DEPTH);
	Vec3 tmp = matrix * topLeft;
	if (tmp.x < 1 && tmp.y < 1 && tmp.z > 0)
		return true;
	tmp = matrix * botRight;
	if (tmp.x < 1 && tmp.y < 1 && tmp.z > 0)
		return true;
	tmp = matrix * Vec3(topLeft.x, 0, botRight.z);
	if (tmp.x < 1 && tmp.y < 1 && tmp.z > 0)
		return true;
	tmp = matrix * Vec3(botRight.x, 0, topLeft.z);
	if (tmp.x < 1 && tmp.y < 1 && tmp.z > 0)
		return true;
	return false;
}

void World::setCamera(Camera newCamera)
{
	camera = newCamera;
}

Camera& World::getCamera()
{
	return camera;
}
