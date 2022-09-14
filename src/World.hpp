/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   World.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/07/21 18:11:54 by nathan            #+#    #+#             */
/*   Updated: 2022/09/13 15:44:27 by nallani          ###   ########.fr       */
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

# define VIEW_DISTANCE 160
# define CHUNK_VIEW_DISTANCE (VIEW_DISTANCE / CHUNK_WIDTH)
# define MAX_PRELOAD_DISTANCE ((float)CHUNK_VIEW_DISTANCE + 1)
# define PRELOAD_DISTANCE_DEL ((float)CHUNK_VIEW_DISTANCE + 6)

class World {
public:
	World( void );
	virtual ~World( void );
	void render( void );
	void setCamera(Camera newCamera);
	void changeLight();
	Camera& getCamera();

	void	update();
	// Chunk Manager
	bool updateChunkBuffers(Vec2 newPos);
	void deleteBlock();

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
	void	printPos() const;
	Camera camera;
	Shader* shader;
	bool light = false;
	std::map<Vec2, std::vector<Vec3>> deletedBlocks;

	Vec2 curPos;

	// Chunk Managers
	std::map<Vec2, Chunk*> visibleChunks;
	std::map<Vec2, Chunk*> preLoadedChunks;
};

#endif
