/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Loop.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/24 15:40:25 by nathan            #+#    #+#             */
/*   Updated: 2022/07/22 18:26:42 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iomanip>
#include <unistd.h>
#include "Loop.hpp"
#include <glfw3.h>
#include "Camera.hpp"
#include "ft_vox.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

bool Loop::shouldStop = false;
bool Loop::shift_lock = false;
double Loop::frameTime = 0.0f;
std::string Loop::lastFps = "FPS: 60";
const double Loop::refreshingRate = 1.0 / 60.0;
std::vector<Object*> Loop::objects = {};
World* Loop::world = nullptr;
double Loop::mouseX = 0.0;
double Loop::mouseY = 0.0;
double Loop::fpsRefreshTime = 0.0;
unsigned char Loop::frameCount = 0;

#define CAMERA_MOUVEMENT_SPEED 1.0f
#define REFRESH_FPS_RATE 0.5


void Loop::loop()
{
	Shader *simpleDepthShader = new Shader("voxCube.vert", "depth.frag");

	Shader *debugQuadShader = new Shader("debug.vert", "debug.frag");
	debugQuadShader->use();
	debugQuadShader->setFloat("near_plane", NEAR);
	debugQuadShader->setFloat("far_plane", FAR);
	debugQuadShader->setFloat("depthMap", 0);

	glfwSetTime(0);
	glfwSetKeyCallback(appWindow::getWindow(), Loop::keyCallback);
	glfwGetCursorPos(appWindow::getWindow(), &mouseX, &mouseY);
	while (!glfwWindowShouldClose(appWindow::getWindow()))
	{
		double currentTimer = glfwGetTime();
		frameCount++;
		processInput();

		//if (!(world->pause))
			//glClear((world->wireframe ? GL_COLOR_BUFFER_BIT : 0) | GL_DEPTH_BUFFER_BIT);

		glBindFramebuffer(GL_FRAMEBUFFER, ResourceManager::framebuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
		glEnable(GL_DEPTH_TEST);
		
		// 1. render depth of scene to texture (from light's perspective)
        // --------------------------------------------------------------
        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        float near_plane = 1.0f, far_plane = 7.5f;
        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        lightView = glm::lookAt(glm::vec3(0, 256, 0), glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;
        // render scene from light's point of view
        simpleDepthShader->use();
        simpleDepthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
		
        glViewport(0, 0, 1024, 1024);
		glClear(GL_DEPTH_BUFFER_BIT);

		if (world && !(world->pause))
		{
			world->update();
			world->render(simpleDepthShader);
		}

        glViewport(0, 0, 1920, 1024);
		glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
		glDisable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// // 2. render scene as normal using the generated depth/shadow map  
        // // --------------------------------------------------------------
		// world->getShader()->setMat4("lightSpaceMatrix", lightSpaceMatrix);
        // glActiveTexture(GL_TEXTURE0 + BLOCK_BEDROCK + 1);
        // glBindTexture(GL_TEXTURE_2D, ResourceManager::colorbuffer);

		// if (world && !(world->pause))
		// {
		// 	world->update();
		// 	world->render(NULL);
		// }
		// if (fpsRefreshTime + 1.f < currentTimer)
		// {
		// 	double fps = (float)frameCount / (currentTimer - fpsRefreshTime);
		// 	frameCount = 0;
		// 	fpsRefreshTime = currentTimer;
		// 	lastFps = "FPS: " + std::to_string((int)fps).substr(0, 4);
		// }
		// PRINT_TO_SCREEN(lastFps);
		// TextManager::tickPrint();
		// glFinish();

		debugQuadShader->use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ResourceManager::colorbuffer);

		renderQuad();

		glfwSwapBuffers(appWindow::getWindow());

		frameTime = glfwGetTime() - currentTimer;
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
	bool shift = glfwGetKey(appWindow::getWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? !shift_lock : shift_lock;
	if (world)
	{
		world->getCamera().move(forward, backward, left, right, (shift == true ? 10 : 1) * CAMERA_MOUVEMENT_SPEED);
		if (glfwGetKey(appWindow::getWindow(), GLFW_KEY_SPACE) == GLFW_PRESS)
			world->getCamera().moveUp(frameTime * 10.0f);
		if (glfwGetKey(appWindow::getWindow(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
			world->getCamera().moveDown(frameTime * 10.0f);
	}

	if (glfwGetWindowAttrib(appWindow::getWindow(), GLFW_FOCUSED) && !(world->pause))
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

void Loop::KeyCallbackProcess(bool keysPressed[389])
{
	for (int i = 0; i < 389; i++)
	{
		if (!keysPressed[i])
			continue;
		if (i == GLFW_KEY_P)
		{
			world->pause = !world->pause;
			if (world->pause)
				glfwSetInputMode(appWindow::getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			else
			{
				glfwSetCursorPos(appWindow::getWindow(), appWindow::getWindowWidth() * 0.5f, appWindow::getWindowHeight() * 0.5f);
				glfwSetInputMode(appWindow::getWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
			}
		}
		if (i == GLFW_KEY_O)
		{
			world->freeze = !world->freeze;
			if (world->freeze)
				printf("World freezed\n");
			else
				printf("World resumed\n");
		}
		if (i == GLFW_KEY_I)
		{
			world->blocFreeze = !world->blocFreeze;
			if (world->blocFreeze)
				printf("World blocks freezed\n");
			else
				printf("World blocks resumed\n");
		}
		if (i == GLFW_KEY_Z)
		{
			world->wireframe = !world->wireframe;
			if (world->wireframe)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		if (i == GLFW_KEY_KP_1 || i == GLFW_KEY_KP_2)
		{
			world->updateSkyboxDEBUG(0, GLFW_KEY_KP_1 ? 0.1f : -0.1f, 0, 0);
		}
		if (i == GLFW_KEY_KP_4 || i == GLFW_KEY_KP_5)
		{
			world->updateSkyboxDEBUG(i == GLFW_KEY_KP_4 ? 0.001f : -0.001f, 0, 0, 0);
		}
		if (i == GLFW_KEY_KP_7 || i == GLFW_KEY_KP_8)
		{
			world->updateSkyboxDEBUG(0, 0, i == GLFW_KEY_KP_7 ? 1 : -1, 0);
		}
		if (i == GLFW_KEY_KP_9 || i == GLFW_KEY_KP_6)
		{
			world->updateSkyboxDEBUG(0, 0, 0, i == GLFW_KEY_KP_9 ? 1 : -1);
		}
		if (i == GLFW_KEY_TAB)
			shift_lock = !shift_lock;
	}
}

void Loop::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	(void)scancode;
	(void)mods;// might use for speed of camera

	if (window != appWindow::getWindow())
		return;
	if (key == GLFW_KEY_R && action == GLFW_PRESS && world)
		world->getCamera().reset();


	static bool keysPressed[389] = {false};

	keysPressed[key] = action;
	KeyCallbackProcess(keysPressed);
}

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void Loop::renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ResourceManager::colorbuffer);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}
