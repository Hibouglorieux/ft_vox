/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Faces.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/02 17:36:04 by nallani           #+#    #+#             */
/*   Updated: 2022/09/02 18:30:29 by nallani          ###   ########.fr       */
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
