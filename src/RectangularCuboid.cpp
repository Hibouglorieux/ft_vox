/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RectangularCuboid.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/08 17:52:01 by nathan            #+#    #+#             */
/*   Updated: 2022/09/02 22:42:38 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RectangularCuboid.hpp"
#include "stb_image.h"

bool RectangularCuboid::initialized = false;
GLuint RectangularCuboid::VAO = 0;
GLuint RectangularCuboid::VBO = 0;
GLuint RectangularCuboid::texturePosBuf = 0;

// TODO : We drop instanting draws. We will be using greedy meshing in order to
//		  draw bigger mesh to lessen the vertice/tri count.
//		  It will probably result in more draw call though.

void RectangularCuboid::initialize()
{
	if (initialized)
		return;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &texturePosBuf);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	float vertices[] = {
		// Back face
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right         
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
										  // Front face
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
										  // Left face
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
										  // Right face
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right         
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left     
										 // Bottom face
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
										  // Top face
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right     
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  // bottom-left        
	};

	float texturePos[] = {
		// 3d texture coordinates
		//Back face
		-1.f, -1.f, -1.f, 
		1.f,  1.f, -1.f, 
		1.f, -1.f, -1.f,          
		1.f,  1.f, -1.f, 
		-1.f, -1.f, -1.f, 
		-1.f,  1.f, -1.f, 
		// Front face
		-1.f, -1.f,  1.f, 
		1.f, -1.f,  1.f, 
		1.f,  1.f,  1.f, 
		1.f,  1.f,  1.f, 
		-1.f,  1.f,  1.f, 
		-1.f, -1.f,  1.f, 
		// Left face
		-1.f,  1.f,  1.f, 
		-1.f,  1.f, -1.f, 
		-1.f, -1.f, -1.f, 
		-1.f, -1.f, -1.f, 
		-1.f, -1.f,  1.f, 
		-1.f,  1.f,  1.f, 
		// Right face
		1.f,  1.f,  1.f, 
		1.f, -1.f, -1.f, 
		1.f,  1.f, -1.f,       
		1.f, -1.f, -1.f, 
		1.f,  1.f,  1.f, 
		1.f, -1.f,  1.f,     
		// Bottom face
		-1.f, -1.f, -1.f, 
		1.f, -1.f, -1.f, 
		1.f, -1.f,  1.f, 
		1.f, -1.f,  1.f, 
		-1.f, -1.f,  1.f, 
		-1.f, -1.f, -1.f, 
		// Top face
		-1.f,  1.f, -1.f, 
		1.f,  1.f,  1.f, 
		1.f,  1.f, -1.f,   
		1.f,  1.f,  1.f, 
		-1.f,  1.f, -1.f, 
		-1.f,  1.f,  1.f         
	};

	// vertex positions
	glEnableVertexAttribArray(0);	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
	//glEnableVertexAttribArray(4);
	//glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(sizeof(float) * 5 * 6 * 6));
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);  
	glBindBuffer(GL_ARRAY_BUFFER, texturePosBuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texturePos), texturePos, GL_STATIC_DRAW);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindVertexArray(0);
	initialized = true;
}

void RectangularCuboid::clear() 
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glDeleteBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, texturePosBuf);
	glDeleteBuffers(1, &texturePosBuf);
	glDeleteVertexArrays(1, &VAO);
	initialized = false;
}

//  unused, drawInstance is called instead
void RectangularCuboid::draw(Vec3& pos, Shader* shader, Texture* texture)
{
	initialize();
	shader->use();
	glUniform1i(glGetUniformLocation(shader->getID(), "instanced"), 0);
	Matrix modelMat = Matrix::createTranslationMatrix(pos);
	glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "model"), 1, GL_TRUE, modelMat.exportForGL());
	(void)texture;
	//glBindTexture(GL_TEXTURE_2D, texture->getID());
	//glUniform1i(glGetUniformLocation(shader->getID(), "texture0"), 0);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void RectangularCuboid::drawInstance(Shader* shader, GLuint positionVBO, GLuint typeVBO, unsigned int count)
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
	glEnableVertexAttribArray(2);	
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), (void*)(0));
	glVertexAttribDivisor(2, 1); // on index2, the array is updated on each different instance (second argument == 1) (otherwise it wouldt be updated)

	glBindBuffer(GL_ARRAY_BUFFER, typeVBO);
	glEnableVertexAttribArray(5);
	glVertexAttribIPointer(5, 1, GL_INT, sizeof(GLint), (void*)(0));
	glVertexAttribDivisor(5, 1);

	glUniform1i(glGetUniformLocation(shader->getID(), "color"), GL_FALSE);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 36, count);
}

void RectangularCuboid::drawFace(Shader* shader, GLuint positionVBO, GLuint typeVBO, unsigned int count, const std::vector<char>& visibleFaces)
{
	glBindVertexArray(VAO);


	glUniform1i(glGetUniformLocation(shader->getID(), "color"), GL_FALSE);
	for (unsigned int i = 0; i < count; i++)
	{
		glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
		glEnableVertexAttribArray(2);	
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), (void*)(sizeof(float) * 3 * i));
		glVertexAttribDivisor(2, 1);
		glBindBuffer(GL_ARRAY_BUFFER, typeVBO);
		glEnableVertexAttribArray(5);
		glVertexAttribIPointer(5, 1, GL_INT, sizeof(GLint), (void*)(sizeof(int) * i));
		glVertexAttribDivisor(5, 1);
		if (visibleFaces[i] & BACK_NEIGHBOUR)
			glDrawArrays(GL_TRIANGLES, BACK_FACE, 6);
		if (visibleFaces[i] & FRONT_NEIGHBOUR)
			glDrawArrays(GL_TRIANGLES, FRONT_FACE, 6);
		if (visibleFaces[i] & LEFT_NEIGHBOUR)
			glDrawArrays(GL_TRIANGLES, LEFT_FACE, 6);
		if (visibleFaces[i] & RIGHT_NEIGHBOUR)
			glDrawArrays(GL_TRIANGLES, RIGHT_FACE, 6);
		if (visibleFaces[i] & BOTTOM_NEIGHBOUR)
			glDrawArrays(GL_TRIANGLES, BOTTOM_FACE, 6);
		if (visibleFaces[i] & UP_NEIGHBOUR)
			glDrawArrays(GL_TRIANGLES, TOP_FACE, 6);
	}
}

void RectangularCuboid::drawFaceInstance(Shader* shader, GLuint positionVBO, GLuint typeVBO, unsigned int count, GLuint facesVBO)
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
	glEnableVertexAttribArray(2);	
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), (void*)(0));
	glVertexAttribDivisor(2, 1); // on index2, the array is updated on each different instance (second argument == 1) (otherwise it wouldt be updated)

	glBindBuffer(GL_ARRAY_BUFFER, typeVBO);
	glEnableVertexAttribArray(5);
	glVertexAttribIPointer(5, 1, GL_INT, sizeof(GLint), (void*)(0));
	glVertexAttribDivisor(5, 1);

	glBindBuffer(GL_ARRAY_BUFFER, facesVBO);
	glEnableVertexAttribArray(6);
	glVertexAttribIPointer(6, 1, GL_UNSIGNED_INT, sizeof(GLuint), (void*)(0));
	glVertexAttribDivisor(6, 1);

	glUniform1i(glGetUniformLocation(shader->getID(), "color"), GL_FALSE);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 36, count);
}

/*void RectangularCuboid::drawQuad(Shader* shader, GLuint positionVBO, GLuint typeVBO)
{
	glBindVertexArray(VAO);

	glUniform1i(glGetUniformLocation(shader->getID(), "color"), GL_FALSE);
	glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
	glEnableVertexAttribArray(2);	
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), (void*)(sizeof(float) * 3));
	glVertexAttribDivisor(2, 1);
	glBindBuffer(GL_ARRAY_BUFFER, typeVBO);
	glEnableVertexAttribArray(5);
	glVertexAttribIPointer(5, 1, GL_INT, sizeof(GLint), (void*)(sizeof(int)));
	glVertexAttribDivisor(5, 1);

//	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 36, 2);
}*/

/*
void Quad::initialize(Vec3 bottomLeft, Vec3 topLeft, Vec3 topRight,
		Vec3 bottomRight, int width, int height)
{
	float vertices[] = {
		0, 0, 0, 0, 0
	};

	glBindVertexArray(0);
}
*/
