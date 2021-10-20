/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RectangularCuboid.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/08 17:52:01 by nathan            #+#    #+#             */
/*   Updated: 2021/10/20 12:16:09 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RectangularCuboid.hpp"
#include "stb_image.h"

bool RectangularCuboid::initialized = false;
GLuint RectangularCuboid::VAO = 0;
GLuint RectangularCuboid::VBO = 0;
GLuint RectangularCuboid::transformBuffer = 0;

void RectangularCuboid::initialize()
{
	if (initialized)
		return;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &transformBuffer);
  
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
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // bottom-left        
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);  

    // vertex positions
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
	glBindVertexArray(0);
	initialized = true;
}

void RectangularCuboid::clear() 
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
	glInvalidateBufferData(VBO);
	initialized = false;
}

void RectangularCuboid::draw(Vec3& pos, Shader* shader, Texture* texture)
{
	initialize();
	shader->use();
	glUniform1i(glGetUniformLocation(shader->getID(), "instanced"), 0);
	Matrix modelMat = Matrix::createTranslationMatrix(pos);
    glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "model"), 1, GL_TRUE, modelMat.exportForGL());
	glBindTexture(GL_TEXTURE_2D, texture->getID());
	glUniform1i(glGetUniformLocation(shader->getID(), "texture0"), 0);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void RectangularCuboid::drawInstance(Shader* shader, Texture* texture,
		GLfloat *instanceTransforms, unsigned int count)
{
	initialize();

    glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, transformBuffer);
	glBufferData(GL_ARRAY_BUFFER, count * 3 * sizeof(float),
			instanceTransforms, GL_STATIC_DRAW);

    glEnableVertexAttribArray(2);	
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), (void*)(0));

	glVertexAttribDivisor(2, 1);

	glUniform1i(glGetUniformLocation(shader->getID(), "instanced"), 1);
	glBindTexture(GL_TEXTURE_2D, texture->getID());
	glUniform1i(glGetUniformLocation(shader->getID(), "texture0"), 0);
    glBindVertexArray(VAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 36, count);
}
