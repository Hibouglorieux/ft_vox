/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   appWindow.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <nallani@student.s19.be>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/24 01:55:24 by nathan            #+#    #+#             */
/*   Updated: 2021/10/15 14:44:57 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef APP_WINDOW_CLASS_H
# define APP_WINDOW_CLASS_H

#include "Utilities.h"
#include <GL/glew.h>
#include <glfw3.h>

class appWindow {
public:
	static int init();
	static GLFWwindow* getWindow();
	static void getWindowSize(int* width, int* height);
	static int getWindowWidth();
	static int getWindowHeight();
private:
	static bool initialized;
	static GLFWwindow* window;
	static const GLFWvidmode* mode;
};

#endif
