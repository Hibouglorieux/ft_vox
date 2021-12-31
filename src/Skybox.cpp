/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Skybox.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/20 18:53:42 by nathan            #+#    #+#             */
/*   Updated: 2021/12/03 17:55:18 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Skybox.hpp"
#include "Matrix.hpp"

Texture* Skybox::texture = nullptr;
Shader* Skybox::shader = nullptr;
GLuint Skybox::VAO = 0;
GLuint Skybox::VBO = 0;

void Skybox::initialize()
{
	std::vector<std::string> names = 
	{{"skybox/right.jpg"},
	{"skybox/left.jpg"},
	{"skybox/top.jpg"},
	{"skybox/bottom.jpg"},// TODO temporary not used to see heightmap instead
	{"skybox/front.jpg"},
	{"skybox/back.jpg"}};
	
	// heightmap call
	//BigHeightMap *bigHeightMap = VoxelGenerator::createBigMap(6, 2, 0.5);
	//HeightMap *heightmap = VoxelGenerator::createMap(1, 1);
	//texture = new Texture(names, *bigHeightMap);

	//texture = new Texture(names, *caveMap);
	texture = new Texture(true);// noiseTest
	 
	glActiveTexture(GL_TEXTURE0 + TEXTURECOUNT);// TODO this is a shortcut and is simply added as the last array of textures using, should be included in the enum or have a better variable name
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture->getID());
	shader = new Shader("skybox.vert", "skybox.frag");

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
  
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

	float vertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);  
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindVertexArray(0);
}

float Skybox::ToDELETEClamp(float x, float low, float high)
{
	if (x < low)
		return low;
	if (x > high)
		return high;
	return x;
}

float Skybox::ToDELETESmoothStep(float a, float b, float x, float c1, float c2)
{
	x = ToDELETEClamp((x - a) / (b - a), c1, c2);
	return x * x * (3 - 2 * x);
}

void Skybox::draw(Matrix& precalculatedMat)
{
	shader->use();
	glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "precalcMat"), 1, GL_TRUE, precalculatedMat.exportForGL());
	glUniform1i(glGetUniformLocation(shader->getID(), "skyboxTexture"), TEXTURECOUNT);

	//  TODO :	Keep the sun/moon update move outside of this file (put it in world)
	//			And make the sun displacment stable (unrelated to framerate)
	static float i = 0;
	float intensity = 1.0;
	Matrix rot = Matrix::createRotationMatrix(Matrix::RotationDirection::X, i);
	i = (i + 0.05);
	// Sun intensity will vary like :
	// 0   -> 180  : Decrease
	// 180 -> 360  : Increase
	if (i <= 180.0)
		intensity = ToDELETESmoothStep(1, 0, i / 180.0, 0, 1); // Decrease toward 0
	else
		intensity = ToDELETESmoothStep(0, 1, (i - 180) / 180 , 0, 1); // Increase toward 1
	if (i > 360.0)
		i = 0;
	Vec3 sunPos = rot * Vec3(0, 3000, 0);

	glUniform3f(glGetUniformLocation(shader->getID(), "sunPos"), sunPos.x, sunPos.y, sunPos.z);
	glUniform1f(glGetUniformLocation(shader->getID(), "sunIntensity"), intensity);
	
	glUniform4f(glGetUniformLocation(shader->getID(), "sunColor"), 1, 1, 1, 1.0);
	glUniform4f(glGetUniformLocation(shader->getID(), "moonColor"), 0.15, 0, 0.15+0.07, 1.0);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Skybox::clear()
{
	delete texture;
	delete shader;
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
	glInvalidateBufferData(VBO);
}
