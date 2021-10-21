/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   World.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/07/21 18:11:54 by nathan            #+#    #+#             */
/*   Updated: 2021/10/21 13:43:52 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef WORLD_CLASS_H
# define WORLD_CLASS_H

# include "Object.hpp"
# include "Chunk.hpp"
# include "Camera.hpp"
# include <thread>
#include "Skybox.hpp"

# define VIEW_DISTANCE 160
# define ROW_OF_CHUNK 8

class World {
public:
	World( void );
	virtual ~World( void );
	void render( void );
	void addObject(Object* newobj);
	std::vector<Object*>& getObjects();
	void setCamera(Camera newCamera);
	Camera& getCamera();
private:
	void onEnterAnim();
	void onLeaveAnim();
	Camera camera;
	std::vector<Object*> objects;
	Shader* shader;
};

#endif
