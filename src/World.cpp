/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   World.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/07/21 18:11:30 by nathan            #+#    #+#             */
/*   Updated: 2022/09/02 22:18:18 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "World.hpp"
#include <array>
#include "RectangularCuboid.hpp"
#include <chrono>
#include <unistd.h>
#include <algorithm>
#include "TextManager.hpp"

#define WAITING_THREAD_TIME (0.1 * SEC_TO_MICROSEC)

World::World()
{
	shader = new Shader();
	shader->use();
	glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "projection"), 1, GL_TRUE, Object::getProjMat().exportForGL());
	visibleChunks = {};
	preLoadedChunks = {};
	curPos = Chunk::worldCoordToChunk(camera.getPos());

	Skybox::initialize(0.0013f, 1.2f, 1, 0);
	
	auto initNewChunk = [](Chunk *chnk) {
		chnk->initChunk();
	};
	auto positions = getPosInRange(curPos, 0, CHUNK_VIEW_DISTANCE);
	for (auto pos : positions)
	{
			Chunk* chnk = new Chunk(pos.x, pos.y, &camera);
			visibleChunks.insert(std::pair<Vec2, Chunk*>(pos, chnk));
	}
	positions.clear();
	positions = getPosInRange(curPos, CHUNK_VIEW_DISTANCE, MAX_PRELOAD_DISTANCE);
	for (auto pos : positions)
	{
		if (visibleChunks.find(pos) == visibleChunks.end())
		{
			Chunk* chnk = new Chunk(pos.x, pos.y, &camera);
			preLoadedChunks.insert(std::pair<Vec2, Chunk*>(pos, chnk));
		}
	}
	for (auto it : visibleChunks)
	{
		auto allocatedNeighbors = getAllocatedNeighbours(it.first);
		it.second->setNeighbors(allocatedNeighbors);

		std::thread worker(initNewChunk, it.second);
		worker.detach();
	}
	for (auto it : preLoadedChunks)
	{
		auto allocatedNeighbors = getAllocatedNeighbours(it.first);
		it.second->setNeighbors(allocatedNeighbors);

		std::thread worker(initNewChunk, it.second);
		worker.detach();
	}
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

void World::updateSkyboxDEBUG(float _freq, float _amp, int _octaves, int _y)
{
	amp += _amp;
	freq += _freq;
	octaves += _octaves;
	y += _y;
	printf("%f %f %i %i\n", _freq, _amp, _octaves, _y);
	Skybox::clear();
	Skybox::initialize(freq, amp, octaves, y);
}

void World::printPos() const
{
	std::stringstream ss;
	ss << "currentPos is chunk: {" << curPos.x <<  ", "<< curPos.y << "}";
	Vec3 blocPos;
	blocPos.x = (int)(camera.getPos().x + 0.5 - curPos.x * CHUNK_WIDTH);
	blocPos.z = (int)(camera.getPos().z + 0.5 - curPos.y * CHUNK_DEPTH);
	if (blocPos.x == CHUNK_WIDTH)
		blocPos.x = 0;
	if (blocPos.z == CHUNK_DEPTH)
		blocPos.z = 0;
	ss << " at block pos x: " << blocPos.x << " y: " << (int)(camera.getPos().y + 0.5) << " z: " << blocPos.z;
	//PRINT_TO_SCREEN(ss.str());
}

void World::render()
{
	//std::cout << camera.getPos().x << "," << camera.getPos().z << std::endl;

	Matrix precalculatedMat = Object::getProjMat() * camera.getMatrix();

	// draw skybox
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	Skybox::playerPos = camera.getPos();
	Skybox::draw(precalculatedMat);

	// draw chunks
	glEnable(GL_DEPTH_TEST);
	glDepthMask(true);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);// renders only visible squares of cubes
	shader->use();
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
	std::vector<std::pair<Vec2, Chunk*>> chunksToRender;
	if (!freeze)
	{
		for (auto it : visibleChunks)
		{
			Chunk* chnk = it.second;
			const Vec2& pos = it.first;
			if (shouldBeRendered(pos, chnk, precalculatedMat))// TODO not working properly because it only takes 4 points on the same Y axis, either complexify it or keep it only for Z < 1
			{
				//printf("Rendering : (%i, %i)\n", pos.x, pos.y);
				std::pair<Vec2, Chunk*> tmpPair(pos - curPos, chnk); // relative to playerPos
				chunksToRender.push_back(tmpPair);

				//threads.push_back(std::thread(chunkVisibilityUpdate, chnk, blocFreeze));

				//chunksToRender.push_back(std::make_pair<Vec2, Chunk*>(pos - curPos, chnk));
				//chunksToRender[it.first - curPos] = chnk;
				//chnk->draw(shader);
			}
			//printf("\n");
		}

		Camera clone = camera;
		//printf("\n\n\n");
		std::sort(chunksToRender.begin(), chunksToRender.end(), [/*camera=clone*/](std::pair<Vec2, Chunk*>& a, std::pair<Vec2, Chunk*>& b) {
				return a.first.isSmaller(b.first); // NOTE : Old way, make the closet chunk to us load
				/*Vec3 a3 = Vec3(a.first.x, 0, a.first.y);
				Vec3 b3 = Vec3(b.first.x, 0, b.first.y);
				float value = a3.dot(camera.getDirection());
				float value2 = b3.dot(camera.getDirection());

				return value < value2;*/
				});
		chunksToRenderFix = chunksToRender;
	}
	else
		chunksToRender = chunksToRenderFix;

	for (auto it : chunksToRender)
	{
		//it.second->updateVisibilityByCamera(blocFreeze);
		it.second->draw(shader);
	}

	//std::cout << "rendered: " << chunksToRender.size() << "on total visible chunk: " << visibleChunks.size() << std::endl;
}

void World::update()
{
	Vec2 newChunkPos = Chunk::worldCoordToChunk(camera.getPos());
	if (curPos != newChunkPos && !freeze)
	{
		curPos = newChunkPos;
		updateChunkBuffers(curPos);
	}
	printPos();

}
/**
 * @brief Function used to update the world buffers containing the visible and preloaded chunks.
 * 
 * @param newPos Current position of the camera used to determine buffer next state
 */
void World::updateChunkBuffers(Vec2 newPos)
{
	std::vector<Vec2> posBuffer = {};

	// Clean chunks that are too far from the camera (save RAM)
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
	
	//Transfer current visible chunk to preloaded if necessary (or delete them is needed)
	posBuffer.clear();
	for (auto it : visibleChunks)
	{
		const Vec2& pos = it.first;
		Vec2 relativePos = pos - newPos;

		Chunk*& chunk = it.second;
		if (relativePos.getLength() >= MAX_PRELOAD_DISTANCE && chunk->hasThreadFinished())
		{
			delete chunk;
			posBuffer.push_back(pos);
		}
		else if (relativePos.getLength() >= CHUNK_VIEW_DISTANCE)
		{
			if (preLoadedChunks.find(pos) != preLoadedChunks.end())
			{
				std::cout << "problem : chunk must go to visible but is already there in " << std::endl;
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

	// Add new chunk to be preloaded
	posBuffer.clear();
	posBuffer = getPosInRange(newPos, CHUNK_VIEW_DISTANCE, MAX_PRELOAD_DISTANCE);
	auto initNewChunks = [](std::vector<Chunk*> chunks) { for (auto it : chunks) it->initChunk();};
	std::vector<Chunk*> chunks;
	for (Vec2& preloadedPositions : posBuffer) // TODO Load those in front first, who cares about the chunk behind us
	{
		if (preLoadedChunks.find(preloadedPositions) == preLoadedChunks.end())
		{
			auto allocatedNeighbours = getAllocatedNeighbours(preloadedPositions);
			chunks.push_back(new Chunk(preloadedPositions.x, preloadedPositions.y, &camera, allocatedNeighbours));
			preLoadedChunks.insert(std::pair<Vec2, Chunk*>(preloadedPositions, chunks.back()));
		}
	}
	std::thread worker(initNewChunks, chunks);
	worker.detach();
	auto initNewChunk = [](Chunk *chnk) { chnk->initChunk(); };
	Chunk *chnk = NULL;
		
	
	//move from preloaded to visible // or load if thats not the case ?
	posBuffer = getPosInRange(newPos, 0, CHUNK_VIEW_DISTANCE);
	for (auto key : posBuffer)
	{
		if (visibleChunks.find(key) == visibleChunks.end())
		{
			if (preLoadedChunks.find(key) == preLoadedChunks.end())
			{
				auto allocatedNeighbours = getAllocatedNeighbours(key);
				chnk = new Chunk(key.x, key.y, &camera, allocatedNeighbours);
				visibleChunks.insert(std::pair<Vec2, Chunk*>(key, chnk));
				
				std::thread worker(initNewChunk, chnk);
				worker.detach();
			}
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

bool	World::shouldBeRendered(Vec2 chunkPos, const Chunk* chnk, Matrix& matrix)
{
	(void)chunkPos;
	(void)matrix;

	return (chnk->boundingVolume.isOnFrustum(camera.getFrustum()));
}

void World::setCamera(Camera newCamera)
{
	camera = newCamera;
}

Camera& World::getCamera()
{
	return camera;
}
