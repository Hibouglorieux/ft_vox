/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   World.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/07/21 18:11:30 by nathan            #+#    #+#             */
/*   Updated: 2022/09/13 15:22:03 by nallani          ###   ########.fr       */
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
		Chunk* chnk = new Chunk(pos.x, pos.y, &camera, deletedBlocks);
		visibleChunks.insert(std::pair<Vec2, Chunk*>(pos, chnk));
	}
	positions.clear();
	positions = getPosInRange(curPos, CHUNK_VIEW_DISTANCE, MAX_PRELOAD_DISTANCE);
	for (auto pos : positions)
	{
		if (visibleChunks.find(pos) == visibleChunks.end())
		{
			Chunk* chnk = new Chunk(pos.x, pos.y, &camera, deletedBlocks);
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

void World::deleteBlock()
{
	Vec3 curPos = camera.getPos();
	Vec3 dir = camera.getDirection();
	dir.getNormalized();

	while (curPos.y > 0 && curPos.y < HEIGHT && (camera.getPos() - curPos).getLength() < 50)
	{
		float xClose = abs((dir.x > 0 ? round(curPos.x) + 0.5 - curPos.x : round(curPos.x) - 0.5 - curPos.x));
		float yClose = abs((dir.y > 0 ? round(curPos.y) + 0.5 - curPos.y : round(curPos.y) - 0.5 - curPos.y));
		float zClose = abs((dir.z > 0 ? round(curPos.z) + 0.5 - curPos.z : round(curPos.z) - 0.5 - curPos.z));
		if (xClose == 0)
			xClose = 1;
		if (yClose == 0)
			yClose = 1;
		if (zClose == 0)
			zClose = 1;
		/*
		std::cout << "dir: " << dir << std::endl;
		std::cout << "xClose: " << xClose 
			<< " yClose: " << yClose 
			<< " zClose: " << zClose << std::endl;
			*/

		
		if (abs(dir.x) > 0.0001)
			xClose /= abs(dir.x);
		else
			xClose = std::numeric_limits<float>::infinity();
		if (abs(dir.y) > 0.0001)
			yClose /= abs(dir.y);
		else
			yClose = std::numeric_limits<float>::infinity();
		if (abs(dir.z) > 0.0001)
			zClose /= abs(dir.z);
		else
			zClose = std::numeric_limits<float>::infinity();
		/*
		std::cout << "xClose: " << xClose 
			<< " yClose: " << yClose 
			<< " zClose: " << zClose << std::endl;
			*/
		float factor;
		bool factorx = false;
		bool factory = false;
		bool factorz = false;
		if ((xClose < yClose || yClose == 0) && (xClose < zClose || zClose == 0)&& xClose != 0)
		{
			factor = xClose;
			factorx = true;
		}
		else if ((yClose < xClose || xClose == 0) && (yClose < zClose || zClose == 0) && yClose != 0)
		{
			factory = true;
			factor = yClose;
		}
		else
		{
			factor = zClose;
			factorz = true;
		}
		//std::cout << "factor: " << factor << "from factor " << (factorx ? "x" : factory ? "y" : "z") << std::endl;
		curPos = curPos + (dir * factor);
		//std::cout << "curPos: " << curPos << std::endl;

		Vec3 flooredPos = Vec3((int)(factorx ? (dir.x < 0 ? floor(curPos.x) : ceil(curPos.x)) : round(curPos.x)),
				(int)(factory ? (dir.y < 0 ? floor(curPos.y) : ceil(curPos.y)) : round(curPos.y)),
				(int)(factorz ? (dir.z < 0 ? floor(curPos.z) : ceil(curPos.z)) : round(curPos.z)));
		Vec2 chunkPos = Chunk::worldCoordToChunk(flooredPos);
		if (visibleChunks.find(chunkPos) == visibleChunks.end())
			return;
		Vec3 relativeFlooredPos = flooredPos - visibleChunks[chunkPos]->getPos();
		//std::cout << "bloc in chunk pos is: " << relativeFlooredPos << std::endl;;
		if (visibleChunks[chunkPos]->deleteBlock(relativeFlooredPos))
		{
			if (deletedBlocks.find(chunkPos) == deletedBlocks.end())
				deletedBlocks.insert({chunkPos, {}});
			deletedBlocks[chunkPos].push_back(relativeFlooredPos);
			//std::cout << "destroyed at: "<< relativeFlooredPos << "in chunk: " << chunkPos.toString() <<  std::endl;
			if (relativeFlooredPos.x == 0 && visibleChunks.find({chunkPos.x - 1, chunkPos.y}) != visibleChunks.end())
				visibleChunks.at({chunkPos.x - 1, chunkPos.y})->updateWithNeighbourBlockDestroyed(Vec3(CHUNK_WIDTH - 1, relativeFlooredPos.y, relativeFlooredPos.z));
			if (relativeFlooredPos.x == CHUNK_WIDTH - 1 && visibleChunks.find({chunkPos.x + 1, chunkPos.y}) != visibleChunks.end())
				visibleChunks.at({chunkPos.x + 1, chunkPos.y})->updateWithNeighbourBlockDestroyed(Vec3(0, relativeFlooredPos.y, relativeFlooredPos.z));
			if (relativeFlooredPos.z == 0 && visibleChunks.find({chunkPos.x, chunkPos.y - 1}) != visibleChunks.end())
				visibleChunks.at({chunkPos.x, chunkPos.y - 1})->updateWithNeighbourBlockDestroyed(Vec3(relativeFlooredPos.x, relativeFlooredPos.y, CHUNK_DEPTH - 1));
			if (relativeFlooredPos.z == CHUNK_DEPTH - 1 && visibleChunks.find({chunkPos.x, chunkPos.y + 1}) != visibleChunks.end())
				visibleChunks.at({chunkPos.x, chunkPos.y + 1})->updateWithNeighbourBlockDestroyed(Vec3(relativeFlooredPos.x, relativeFlooredPos.y, 0));
			return;
		}
	}
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
	PRINT_TO_SCREEN(ss.str());
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

	glUniform3f(glGetUniformLocation(shader->getID(), "playerPos"), camera.getPos().x, camera.getPos().y, camera.getPos().z);
	glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "precalcMat"), 1, GL_TRUE, precalculatedMat.exportForGL());
	glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "view"), 1, GL_TRUE, camera.getMatrix().exportForGL());

	int allTextures[8] = {BLOCK_SAND, BLOCK_DIRT, BLOCK_GRASS, BLOCK_GRASS_SNOW, BLOCK_STONE, BLOCK_SNOW, BLOCK_STONE_SNOW, BLOCK_BEDROCK};
	glUniform1iv(glGetUniformLocation(shader->getID(), "allTextures"), 8, allTextures);

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

void World::changeLight()
{
	light = !light;
	Skybox::changeLight(light);
	shader->use();
	glUniform1i(glGetUniformLocation(shader->getID(), "light"), light);
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
	PRINT_TO_SCREEN(camera.getPos().toString());
	PRINT_TO_SCREEN(camera.getDirection().toString());

}
/**
 * @brief Function used to update the world buffers containing the visible and preloaded chunks.
 * 
 * @param newPos Current position of the camera used to determine buffer next state
 */
bool World::updateChunkBuffers(Vec2 newPos)
{
	std::vector<Vec2> posBuffer = {};
	bool modified = false;

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
			modified = true;
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
			chunks.push_back(new Chunk(preloadedPositions.x, preloadedPositions.y, &camera, allocatedNeighbours, deletedBlocks));
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
			modified = true;
			if (preLoadedChunks.find(key) == preLoadedChunks.end())
			{
				auto allocatedNeighbours = getAllocatedNeighbours(key);
				chnk = new Chunk(key.x, key.y, &camera, allocatedNeighbours, deletedBlocks);
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
	return modified;
}

std::vector<Vec2> World::getPosInRange(Vec2 center, float minDistance, float maxDistance)
{
	std::vector<Vec2> Positions;
	maxDistance++;
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
