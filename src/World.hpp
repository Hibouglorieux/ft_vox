/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   World.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/07/21 18:11:54 by nathan            #+#    #+#             */
/*   Updated: 2021/10/15 13:02:15 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef WORLD_CLASS_H
# define WORLD_CLASS_H

#include "Object.hpp"
#include "Camera.hpp"

class World {
public:
	World( void );
	virtual ~World( void );
	void render( void );
	void addObject(Object* newobj);
	std::vector<Object*>& getObjects();
	void setCamera(Camera newCamera);
	void clearObject(std::string ID);
	Camera& getCamera();
private:
	void onEnterAnim();
	void onLeaveAnim();
	Camera camera;
	std::vector<Object*> objects;
};

#endif
