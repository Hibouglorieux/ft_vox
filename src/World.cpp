/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   World.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/07/21 18:11:30 by nathan            #+#    #+#             */
/*   Updated: 2021/10/22 14:53:36 by nathan           ###   ########.fr       */
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
	visibleChunks = {};
	preLoadedChunks = {};
	curPos = Chunk::worldCoordToChunk(camera.getPos());

	Skybox::initialize();
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
	//updateVisibleChunks();
}

World::~World()
{
	for (auto it : objects)
	{
		delete it;
	}
	objects.clear();
	delete shader;
	Skybox::clear();
}

void World::render()
{
	//std::cout << camera.getPos().x << "," << camera.getPos().z << std::endl; 

	/*if (camera.getChunkUpdate())
		updateVisibleChunks();*/
	Matrix precalculatedMat = Object::getProjMat() * camera.getMatrix();


	// draw skybox
	Skybox::draw(precalculatedMat);


	
	// draw chunks
	shader->use();
	glEnable(GL_DEPTH_TEST);
	glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "precalcMat"), 1, GL_TRUE, precalculatedMat.exportForGL());
	glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "view"), 1, GL_TRUE, camera.getMatrix().exportForGL());
	for (Object* object : objects)
	{
		object->draw(shader);
	}
	for (auto it : visibleChunks)
	{
		Chunk*& chnk = it.second;
		chnk->draw(shader);
	}
}

void World::update()
{
	Vec2 newChunkPos = Chunk::worldCoordToChunk(camera.getPos());
	if (curPos != newChunkPos)
	{
		testUpdateChunks(newChunkPos);
		curPos = newChunkPos;
		std::cout << "currentPos : ";
		curPos.print();
		std::cout << "visibleCHunks: ";
		for (auto it : visibleChunks)
			it.first.print();
	}
}

void World::testUpdateChunks(Vec2 newPos)
{
	std::vector<Vec2> posBuffer = {};
	//delete useless chunk
	for (auto it : preLoadedChunks)
	{
		Vec2 pos = it.first;
		Vec2 relativePos = pos - newPos;
		Chunk*& chunk = it.second;
		if (relativePos.getLength() > PRELOAD_DISTANCE_DEL)
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
		if (relativePos.getLength() > CHUNK_VIEW_DISTANCE)
		{
			preLoadedChunks.insert(std::pair<Vec2, Chunk*>(pos, chunk));
			posBuffer.push_back(pos);
		}
	}
	for (Vec2& pos: posBuffer)
	{
		visibleChunks.erase(pos);
	}

	//preload new chunk
	posBuffer = getPosInRange(newPos, MIN_PRELOAD_DISTANCE, MAX_PRELOAD_DISTANCE);
	for (Vec2& preloadedPositions : posBuffer)
	{
		if (preLoadedChunks.find(preloadedPositions) == preLoadedChunks.end())
		{
			Chunk* chnk = new Chunk(preloadedPositions.x, preloadedPositions.y);
			auto initNewChunk = [](Chunk *chnk) {
				chnk->initChunk();
			};

			//initNewChunk(chnk);
			std::thread worker(initNewChunk, chnk);
			worker.detach();
			preLoadedChunks.insert(std::pair<Vec2, Chunk*>(preloadedPositions, chnk));
		}
	}
		
	
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

void World::updateVisibleChunks(void)
{
	std::cout << "visible" << std::endl;
	// TODO : Write updatePreloadedChunks, this function should be good.

	/*std::vector<Chunk*> updatedVisibleChunks = {};
	camera.setChunkUpdate(false);
	// We remove the visibleChunks which are too far according to the view distance
	for (Chunk *chnk: visibleChunks)
	{
		Vec3 chnkPos = chnk->getChunkCenterPos();
		chnkPos.y = 0;
		Vec3 camPos = camera.getPos();
		camPos.y = 0;
		float dst = (chnkPos - camPos).getLength();//(chnk->getPos() - camera.getPos()).getLength();
		if (dst <= VIEW_DISTANCE)
			updatedVisibleChunks.push_back(chnk);
		else
			preLoadedChunks.push_back(chnk);
		/*else if (dst > VIEW_DISTANCE + CHUNK_WIDTH)
			delete chnk;* // TODO : Delete too far chunk
	}
	visibleChunks.clear();
	visibleChunks = updatedVisibleChunks;

	std::vector<Chunk*> updatedPreLoadedChunks = {};
	for (Chunk *chnk: preLoadedChunks)
	{
		float dst = (chnk->getChunkCenterPos() - camera.getPos()).getLength();
		//std::cout << dst << std::endl;
		if (dst <= VIEW_DISTANCE)
			visibleChunks.push_back(chnk);
		else //if (dst > VIEW_DISTANCE && dst <= VIEW_DISTANCE + CHUNK_WIDTH)
			updatedPreLoadedChunks.push_back(chnk);
		/*else // TODO : delete too far chunk
			delete chnk;*
	}
	preLoadedChunks.clear();
	preLoadedChunks = updatedPreLoadedChunks;*/

	updatePreloadedChunks();
}

void World::updatePreloadedChunks(void)
{
	std::cout << "preloaded" << std::endl;
	//camera.setChunkUpdate(false);
	// Now we need to preload new chunks to ease the world render
	// First we need to check which chunk (coordinate) should be generated
	// How many chunks are visible ? (CHUNK_WIDTH && CHUNK_HEIGHT shoud be the same)
	// chunkOneLineCount = (VIEW_DISTANCE / CHUNK_WIDTH) * 2 <- Give us the chunk count in one direction
	// chunkVisibleCount = chunkViCount * chunkViCount
	/*int chunkCount = (VIEW_DISTANCE / CHUNK_WIDTH + 1) * 2;
	int bufferChunkCount = chunkCount * 4; // Gives use one line of chunk as buffer zone
	int	*chunks = new int[bufferChunkCount];
	memset(chunks, 0, bufferChunkCount * sizeof(int));
	// How to iterate in chunks :
	// the first item is the chunk at the bottom left, then for the next sqrt(chunkCount ) it goes to the upper left,
	// then to the right, then bottom right then to the left
	//
	// We will then iterate through the current preloaded chunks and if some are
	// in the buffer zone we do not need to re-generate them.
	//
	// chunks[0] = chunk at (y, z - dst, x - dst)
	// chunks[sqrt(chunkCount)] = chunk at (y, z, x - dst)
	// chunks[sqrt(chunkCount) * 2] = chunk at (y, z, x + dst)
	// chunks[sqrt(chunkCount) * 3] = chunk at (y, z - dst, x + dst)
	// chunks[sqrt(chunkCount) * 4] = chunk at (y, z - dst, x - dst)
	int maxVisibleDist = (VIEW_DISTANCE / CHUNK_WIDTH) + 1;
	Vec3 currentChunk = camera.getChunkPos();
	std::cout << "Chunk count     : " << chunkCount << std::endl;
	std::cout << "Current Chunk   : " << currentChunk.x << ",0," << currentChunk.z << std::endl;
	std::cout << "Limit           : " << currentChunk.x + maxVisibleDist << " or " << currentChunk.x - maxVisibleDist << std::endl;
	for (Chunk *chnk: preLoadedChunks)
	{
		int mod = 0;
		int index = 0;
		Vec3 pos = chnk->getPos();
		// Sides
		if (pos.x / CHUNK_WIDTH >= currentChunk.x / CHUNK_WIDTH - maxVisibleDist
				|| pos.x / CHUNK_WIDTH <= currentChunk.x / CHUNK_WIDTH - maxVisibleDist)
		{
			std::cout << "Preloaded Chunk : " << pos.x / CHUNK_WIDTH << ",0," << pos.z / CHUNK_DEPTH << std::endl;
			mod = pos.x >= currentChunk.x + maxVisibleDist ? 0 : 1;
			index = (abs(pos.z) + pos.z) / CHUNK_DEPTH;
			std::cout << "Index : " << index << std::endl;
			std::cout << "Index : " << index + mod * chunkCount << std::endl;
			chunks[index + mod * chunkCount] = 1;
		}
		if (pos.z / CHUNK_DEPTH >= currentChunk.z / CHUNK_DEPTH - maxVisibleDist
				|| pos.z / CHUNK_DEPTH <= currentChunk.z / CHUNK_DEPTH - maxVisibleDist)
		{
			std::cout << "Preloaded Chunk : " << pos.x / CHUNK_WIDTH << ",0," << pos.z / CHUNK_DEPTH << std::endl;
			mod = pos.z >= currentChunk.z + maxVisibleDist ? 0 : 1;
			index = (abs(pos.x) + pos.x) / CHUNK_WIDTH;
			std::cout << "Index : " << index << std::endl;
			std::cout << "Index : " << index + mod * chunkCount << std::endl;
			chunks[index + mod * chunkCount] = 1;
		}
	}
	for (int i = 0; i < bufferChunkCount; i++)
	{
		if (i % (chunkCount) == 0)
			std::cout << std::endl;
		std::cout << chunks[i] << " ";
	}
	std::cout << std::endl;
	delete [] chunks;*/

	/*
	int chunksCount = ((VIEW_DISTANCE / CHUNK_WIDTH) + 1) * 2;
	int maxVi = ((VIEW_DISTANCE / CHUNK_WIDTH) + 1);
	int	side = chunksCount;
	chunksCount = (chunksCount * chunksCount);
	int	*chunksRegion = new int[chunksCount];
	memset(chunksRegion, 0, chunksCount * sizeof(int));
	//std::cout << chunksCount << std::endl;
	Vec3 currentPos = camera.getChunkPos();
	Vec3 anchorUpperLeft = camera.getChunkPos();
	anchorUpperLeft.x = anchorUpperLeft.x - side;
	anchorUpperLeft.z = anchorUpperLeft.z - side;
	//std::cout << anchorUpperLeft.x << "," << anchorUpperLeft.z << std::endl;
	// Set current bloc in memory from visibleChunks
	int tmp = -1;
	for (Chunk *chnk: visibleChunks)
	{
		Vec3 chnkPos = chnk->getPos();
		chnkPos.x /= CHUNK_WIDTH;
		chnkPos.y /= CHUNK_HEIGHT;
		chnkPos.z /= CHUNK_DEPTH;
		//std::cout << chnkPos.x << "," << chnkPos.z << " | ";
		chnkPos -= anchorUpperLeft;
			int index = (int)(chnkPos.x) * side + (int)(chnkPos.z);
		if ((chnkPos.x >= currentPos.x - anchorUpperLeft.x - maxVi
				&& chnkPos.x <= currentPos.x - anchorUpperLeft.x + maxVi)
				&& (chnkPos.z >= currentPos.z - anchorUpperLeft.z - maxVi
				&& chnkPos.z <= currentPos.z - anchorUpperLeft.z + maxVi))
		{
			if (tmp == -1)
				tmp = index;
			chunksRegion[index - tmp] = 1;
		}
		std::cout << index << std::endl;
	//	std::cout << "Index : " << (int)(chnkPos.x) * side + (int)(chnkPos.z) << std::endl;
		//std::cout << chnkPos.x << "," << chnkPos.z << std::endl;
		//chunksRegion[(int)(chnkPos.x * side) + (int)(chnkPos.z)] = 1;
		//std::cout << chnkPos.x << "," << chnkPos.z << std::endl;
		//chunksRegion[(int)(chnkPos.z) * side + (int)(chnkPos.x)] = 1;
	}
	for (Chunk *chnk: preLoadedChunks)
	{	}
	std::cout << side << std::endl;
	for (int i = 0; i < chunksCount; i++)
	{
		if (i % (side * 2) == 0)
			std::cout << std::endl;
		std::cout << chunksRegion[i] << "  ";
	}
	std::cout << std::endl;
	delete [] chunksRegion;
	*/
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
	camera = newCamera;
}

Camera& World::getCamera()
{
	return camera;
}
