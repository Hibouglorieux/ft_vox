/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Loop.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/24 15:40:25 by nathan            #+#    #+#             */
/*   Updated: 2021/12/03 16:47:00 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iomanip>
#include <unistd.h>
#include "Loop.hpp"
#include <glfw3.h>
#include "Camera.hpp"
#include "ft_vox.h"

bool Loop::shouldStop = false;
double Loop::frameTime = 0.0f;
const double Loop::refreshingRate = 1.0 / 60.0;
std::vector<Object*> Loop::objects = {};
World* Loop::world = nullptr;
double Loop::mouseX = 0.0;
double Loop::mouseY = 0.0;
double Loop::fpsRefreshTime = 0.0;
unsigned char Loop::frameCount = 0;

#define CAMERA_MOUVEMENT_SPEED 0.3f
#define REFRESH_FPS_RATE 0.5


void Loop::loop()
{
	glfwSetTime(0);
	glfwSetKeyCallback(appWindow::getWindow(), Loop::keyCallback);
	glfwGetCursorPos(appWindow::getWindow(), &mouseX, &mouseY);
	while (!glfwWindowShouldClose(appWindow::getWindow()))
	{
		double currentTimer = glfwGetTime();
		frameCount++;
		processInput();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		if (world)
		{
			world->update();
			world->render();
		}
		glFinish();

		glfwSwapBuffers(appWindow::getWindow());

		frameTime = glfwGetTime() - currentTimer;
		if (fpsRefreshTime + 0.5 < currentTimer)
		{
			std::stringstream ss;
			double fps = (float)frameCount / (currentTimer - fpsRefreshTime);
			ss << std::fixed << std::setprecision(1) << fps;
			glfwSetWindowTitle(appWindow::getWindow(), std::string(PROJECT_NAME + std::string(" ") + std::to_string((int)round(fps)) + std::string(" fps")).c_str());
			frameCount = 0;
			fpsRefreshTime = currentTimer;
		}
	}
}

void Loop::processInput()
{
	glfwPollEvents();
	bool forward, backward, left, right;
	forward = backward = left = right = false;
	if (glfwGetKey(appWindow::getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(appWindow::getWindow(), true);
	// camera
	if (glfwGetKey(appWindow::getWindow(), GLFW_KEY_W) == GLFW_PRESS)
		forward = true;
	if (glfwGetKey(appWindow::getWindow(), GLFW_KEY_S) == GLFW_PRESS)
		backward = true;
	if (glfwGetKey(appWindow::getWindow(), GLFW_KEY_D) == GLFW_PRESS)
		right = true;
	if (glfwGetKey(appWindow::getWindow(), GLFW_KEY_A) == GLFW_PRESS)
		left = true;
	bool shift = glfwGetKey(appWindow::getWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? true : false;
	if (world)
	{
		world->getCamera().move(forward, backward, left, right, (shift == true ? 10 : 1)* CAMERA_MOUVEMENT_SPEED);
		if (glfwGetKey(appWindow::getWindow(), GLFW_KEY_SPACE) == GLFW_PRESS)
			world->getCamera().moveUp();
		if (glfwGetKey(appWindow::getWindow(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
			world->getCamera().moveDown();
	}

	if (glfwGetWindowAttrib(appWindow::getWindow(), GLFW_FOCUSED))
	{
		double oldMouseX = mouseX;
		double oldMouseY = mouseY;
		glfwGetCursorPos(appWindow::getWindow(), &mouseX, &mouseY);
		if (world)
			world->getCamera().rotate(mouseX - oldMouseX, mouseY - oldMouseY);
		glfwSetCursorPos(appWindow::getWindow(), appWindow::getWindowWidth() * 0.5f, appWindow::getWindowHeight() * 0.5f);
		glfwGetCursorPos(appWindow::getWindow(), &mouseX, &mouseY);
		glfwSetInputMode(appWindow::getWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	}
	else
		glfwSetInputMode(appWindow::getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Loop::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	(void)scancode;
	(void)mods;// might use for speed of camera

	if (window != appWindow::getWindow())
		return;
	if (key == GLFW_KEY_R && action == GLFW_PRESS && world)
		world->getCamera().reset();
}
