/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Skybox.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/20 18:53:42 by nathan            #+#    #+#             */
/*   Updated: 2021/10/21 15:47:35 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Skybox.hpp"

Texture* Skybox::texture = nullptr;
Shader* Skybox::shader = nullptr;
GLuint Skybox::VAO = 0;
GLuint Skybox::VBO = 0;

void Skybox::initialize()
{
	texture = new Texture( {{"skybox/right.jpg"},
	{"skybox/left.jpg"},
	{"skybox/top.jpg"},
	{"skybox/bottom.jpg"},
	{"skybox/front.jpg"},
	{"skybox/back.jpg"}});
	/* heightmap call
	BigHeightMap bigHeightMap = VoxelGenerator::createBigMap();
	HeightMap heightmap = VoxelGenerator::createMap(1, 1);
	texture = new Texture( {{"skybox/right.jpg"},
	{"skybox/left.jpg"},
	{"skybox/top.jpg"},
	{"skybox/bottom.jpg"},// TODO temporary not used to see heightmap instead
	{"skybox/front.jpg"},
	{"skybox/back.jpg"}}, heightmap);
	 */
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
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture->getID());
	glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "precalcMat"), 1, GL_TRUE, precalculatedMat.exportForGL());
	glUniform1i(glGetUniformLocation(shader->getID(), "texture0"), 0);
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
