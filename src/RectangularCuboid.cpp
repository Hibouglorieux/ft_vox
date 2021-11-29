/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RectangularCuboid.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/08 17:52:01 by nathan            #+#    #+#             */
/*   Updated: 2021/11/29 21:37:39 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RectangularCuboid.hpp"
#include "stb_image.h"

bool RectangularCuboid::initialized = false;
GLuint RectangularCuboid::VAO = 0;
GLuint RectangularCuboid::VBO = 0;
GLuint RectangularCuboid::transformBuffer = 0;
GLuint RectangularCuboid::typeBuffer = 0;

void RectangularCuboid::initialize()
{
	if (initialized)
		return;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &transformBuffer);
    glGenBuffers(1, &typeBuffer);
  
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
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(sizeof(float) * 5 * 6 * 6));
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);  

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

/*  unused, drawInstance is called instead
void RectangularCuboid::draw(Vec3& pos, Shader* shader, Texture* texture)
{
	initialize();
	shader->use();
	glUniform1i(glGetUniformLocation(shader->getID(), "instanced"), 0);
	Matrix modelMat = Matrix::createTranslationMatrix(pos);
    glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "model"), 1, GL_TRUE, modelMat.exportForGL());
	glBindTexture(GL_TEXTURE_2D, texture->getID());
	//glUniform1i(glGetUniformLocation(shader->getID(), "texture0"), 0);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}
*/

void RectangularCuboid::drawInstance(Shader* shader, GLint *instanceTypes,
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

	glBindBuffer(GL_ARRAY_BUFFER, typeBuffer);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(int),
			instanceTypes, GL_STATIC_DRAW);
	for (GLint i = 0; i < count; i++)
	{
		if (instanceTypes[i] != 3)
			std::cout << instanceTypes[i] << std::endl;
	}

    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 1, GL_INT, GL_TRUE, sizeof(int), (void*)(0));

	glUniform1i(glGetUniformLocation(shader->getID(), "color"), GL_FALSE);

    glBindVertexArray(VAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 36, count);
}
