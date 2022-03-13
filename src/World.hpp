/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   World.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/07/21 18:11:54 by nathan            #+#    #+#             */
/*   Updated: 2021/12/17 21:40:02 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef WORLD_CLASS_H
# define WORLD_CLASS_H

# include "Object.hpp"
# include "Chunk.hpp"
# include "Camera.hpp"
# include <thread>
# include "Skybox.hpp"
# include "Vec2.hpp"
# include <map>
# include "ctpl_stl.h"

# define VIEW_DISTANCE 160
# define ROW_OF_CHUNK 8
//# define CHUNK_VIEW_DISTANCE (VIEW_DISTANCE / CHUNK_WIDTH + 1)
# define CHUNK_VIEW_DISTANCE 10
# define MAX_PRELOAD_DISTANCE ((float)CHUNK_VIEW_DISTANCE + 2)
# define PRELOAD_DISTANCE_DEL ((float)CHUNK_VIEW_DISTANCE + 20)

class World {
public:
	World( void );
	virtual ~World( void );
	void render( void );
	void setCamera(Camera newCamera);
	Camera& getCamera();

	void 	updateSkyboxDEBUG(float freq, float amp, int octaves, int y = 0);
	void	update();
	// Chunk Manager
	void updateChunkBuffers(Vec2 newPos);

	bool pause = false;
	bool freeze = false;
	bool blocFreeze = false;
	bool wireframe = false;

	float freq = 0.0013f;
	float amp = 1.2f;
	int octaves = 1;
	int y = 0;

	std::vector<std::pair<Vec2, Chunk*>> chunksToRenderFix;

private:
	bool shouldBeRendered(Vec2 chunkPos, const Chunk* chnk, Matrix& matrix);
	std::vector<Vec2> getPosInRange(Vec2 center, float minDistance, float maxDistance);
	std::vector<std::pair<Vec2, Chunk*>> getAllocatedNeighbours(Vec2 chunkPos);
	Camera camera;
	Shader* shader;

	Vec2 curPos;

	// Chunk Managers
	std::map<Vec2, Chunk*> visibleChunks;
	std::map<Vec2, Chunk*> preLoadedChunks;

	ctpl::thread_pool thread_pool;
};

#endif
