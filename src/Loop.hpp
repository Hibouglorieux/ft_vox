/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Loop.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/24 15:40:30 by nathan            #+#    #+#             */
/*   Updated: 2021/10/15 13:02:17 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef LOOP_CLASS_H
# define LOOP_CLASS_H

#include <vector>
#include "World.hpp"

class Loop {
public:
	static void loop();
	static void addObject(Object* newobj);
	static void setWorld(World* newWorld) {world = newWorld;};
private:
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void scrollCallBack(GLFWwindow* window, double xoffset, double yoffset);
	static void processInput();
	static bool shouldStop;
	static double frameTime;
	static double fpsRefreshTime;
	static unsigned char frameCount;
	static double mouseX, mouseY;
	const static double refreshingRate;
	static std::vector<Object*> objects;
	static World* world;
};

#endif
