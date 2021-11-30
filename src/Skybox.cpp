/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Skybox.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/20 18:53:42 by nathan            #+#    #+#             */
/*   Updated: 2021/11/30 16:42:27 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Skybox.hpp"

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
	BigHeightMap *bigHeightMap = VoxelGenerator::createBigMap(6, 2, 0.5);
	//HeightMap *heightmap = VoxelGenerator::createMap(1, 1);
	//HeightMap* heightmap = VoxelGenerator::createMap(1, 1);
	texture = new Texture(names, *bigHeightMap);
	 
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

void Skybox::draw(Matrix& precalculatedMat)
{
	glDisable(GL_DEPTH_TEST);
	shader->use();
	glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "precalcMat"), 1, GL_TRUE, precalculatedMat.exportForGL());
	glUniform1i(glGetUniformLocation(shader->getID(), "skyboxTexture"), TEXTURECOUNT);
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
