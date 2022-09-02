/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Faces.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/02 17:36:04 by nallani           #+#    #+#             */
/*   Updated: 2022/09/02 22:37:12 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Faces.hpp"

bool	FrontFace::initialized = 0;
GLuint	FrontFace::VAO = 0;
GLuint	FrontFace::VBO = 0;

bool	RightFace::initialized = 0;
GLuint	RightFace::VAO = 0;
GLuint	RightFace::VBO = 0;

bool	BottomFace::initialized = 0;
GLuint	BottomFace::VAO = 0;
GLuint	BottomFace::VBO = 0;

bool	BackFace::initialized = 0;
GLuint	BackFace::VAO = 0;
GLuint	BackFace::VBO = 0;

bool	LeftFace::initialized = 0;
GLuint	LeftFace::VAO = 0;
GLuint	LeftFace::VBO = 0;

bool	TopFace::initialized = 0;
GLuint	TopFace::VAO = 0;
GLuint	TopFace::VBO = 0;

void Face::Initialize()
{
	if (getInitialized())
		return;

	GLuint VAO;
	GLuint VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	SetVAO(VAO);
	SetVBO(VBO);
	// vertex positions
	glBufferData(GL_ARRAY_BUFFER, sizeof(getVerticesData().data), getVerticesData().data, GL_STATIC_DRAW);  
	glEnableVertexAttribArray(0);	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));

	glBindVertexArray(0);
	setInitialized(true);
}

void Face::Clear()
{
	GLuint VAO = GetVAO();
	GLuint VBO = GetVBO();
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
	setInitialized(false);
}

void Face::drawCommon(Shader* shader, GLuint positionVBO, GLuint TextureVBO, unsigned int count)
{
	glBindBuffer(GL_ARRAY_BUFFER, positionVBO);

	glEnableVertexAttribArray(10);
	glVertexAttribPointer(10, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), (void*)(0));
	glVertexAttribDivisor(10, 1); // on index2, the array is updated on each different instance (second argument == 1) (otherwise it wouldt be updated)

	glBindBuffer(GL_ARRAY_BUFFER, TextureVBO);
	glEnableVertexAttribArray(11);
	glVertexAttribIPointer(11, 1, GL_INT, sizeof(GLint), (void*)(0));
	glVertexAttribDivisor(11, 1);

	glUniform1i(glGetUniformLocation(shader->getID(), "color"), GL_TRUE);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 36, count);
}

void FrontFace::draw(Shader* shader, GLuint positionVBO, GLuint TextureVBO, unsigned int count)
{
	glBindVertexArray(VAO);
	Face::drawCommon(shader, positionVBO, TextureVBO, count);
}

void BackFace::draw(Shader* shader, GLuint positionVBO, GLuint TextureVBO, unsigned int count)
{
	glBindVertexArray(VAO);
	Face::drawCommon(shader, positionVBO, TextureVBO, count);
}
void BottomFace::draw(Shader* shader, GLuint positionVBO, GLuint TextureVBO, unsigned int count)
{
	glBindVertexArray(VAO);
	Face::drawCommon(shader, positionVBO, TextureVBO, count);
}

void TopFace::draw(Shader* shader, GLuint positionVBO, GLuint TextureVBO, unsigned int count)
{
	glBindVertexArray(VAO);
	Face::drawCommon(shader, positionVBO, TextureVBO, count);
}

void LeftFace::draw(Shader* shader, GLuint positionVBO, GLuint TextureVBO, unsigned int count)
{
	glBindVertexArray(VAO);
	Face::drawCommon(shader, positionVBO, TextureVBO, count);
}

void RightFace::draw(Shader* shader, GLuint positionVBO, GLuint TextureVBO, unsigned int count)
{
	glBindVertexArray(VAO);
	Face::drawCommon(shader, positionVBO, TextureVBO, count);
}
