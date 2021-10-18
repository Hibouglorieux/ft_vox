/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   World.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/07/21 18:11:54 by nathan            #+#    #+#             */
/*   Updated: 2021/10/18 18:05:32 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef WORLD_CLASS_H
# define WORLD_CLASS_H

#include "Object.hpp"
#include "Camera.hpp"

#include "Texture.hpp"// TODO remove
#include "Shader.hpp"// TODO REMOVE

class World {
public:
	World( void );
	virtual ~World( void );
	void render( void );
	void addObject(Object* newobj);
	std::vector<Object*>& getObjects();
	void setCamera(Camera newCamera);
	void setTexture(Texture* newTexture);
	Camera& getCamera();
private:
	void onEnterAnim();
	void onLeaveAnim();
	Camera camera;
	std::vector<Object*> objects;
	Shader* shader;
	Texture* texture;
};

#endif
