/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   appWindow.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <nallani@student.s19.be>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/24 01:55:24 by nathan            #+#    #+#             */
/*   Updated: 2022/02/25 23:40:23 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef APP_WINDOW_CLASS_H
# define APP_WINDOW_CLASS_H

#include <GL/glew.h>
#include <glfw3.h>

#include <ft2build.h>
#include FT_FREETYPE_H

class appWindow {
public:
	static int init();
	static GLFWwindow* getWindow();
	static void getWindowSize(int* width, int* height);
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static int getWindowWidth();
	static int getWindowHeight();
	static void resetPrintCalls();
	static void	print(const char* str);
private:
	static bool initialized;
	static GLFWwindow* window;
	static const GLFWvidmode* mode;
	static unsigned char printCalls;
	static FT_Library ft;
	static FT_Face face;
};

#endif
