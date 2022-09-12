/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Loop.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/24 15:40:30 by nathan            #+#    #+#             */
/*   Updated: 2022/07/22 18:24:49 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef LOOP_CLASS_H
# define LOOP_CLASS_H

#include <vector>
#include "World.hpp"
#include "TextManager.hpp"

class Loop {
public:
	static void loop();
	static void addObject(Object* newobj);
	static void setWorld(World* newWorld) {world = newWorld;};
private:
	static void KeyCallbackProcess(bool keysPressed[389]);
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void processInput();
	static void renderQuad();
	static bool shouldStop;
	static bool shift_lock;
	static double frameTime;
	static std::string lastFps;
	static double fpsRefreshTime;
	static unsigned char frameCount;
	static double mouseX, mouseY;
	const static double refreshingRate;
	static std::vector<Object*> objects;
	static World* world;
};

#endif
