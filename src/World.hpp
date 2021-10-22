/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   World.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/07/21 18:11:54 by nathan            #+#    #+#             */
/*   Updated: 2021/10/22 16:46:27 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef WORLD_CLASS_H
# define WORLD_CLASS_H

# include "Object.hpp"
# include "Chunk.hpp"
# include "Camera.hpp"
# include <thread>
# include "Skybox.hpp"
# include <map>

# define VIEW_DISTANCE 160
# define ROW_OF_CHUNK 4
# define CHUNK_VIEW_DISTANCE (VIEW_DISTANCE / CHUNK_WIDTH + 1)
# define MAX_PRELOAD_DISTANCE ((float)CHUNK_VIEW_DISTANCE + 2)
# define PRELOAD_DISTANCE_DEL ((float)CHUNK_VIEW_DISTANCE + 4)

class World {
public:
	World( void );
	virtual ~World( void );
	void render( void );
	void setCamera(Camera newCamera);
	Camera& getCamera();

	void	update();
	// Chunk Manager
	void testUpdateChunks(Vec2 newPos);
private:
	std::vector<Vec2> getPosInRange(Vec2 center, float minDistance, float maxDistance);
	Camera camera;
	Shader* shader;

	Vec2 curPos;

	// Chunk Managers
	std::map<Vec2, Chunk*> visibleChunks;
	std::map<Vec2, Chunk*> preLoadedChunks;
};

#endif
