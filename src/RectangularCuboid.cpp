/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RectangularCuboid.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/08 17:52:01 by nathan            #+#    #+#             */
/*   Updated: 2021/10/18 12:20:28 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RectangularCuboid.hpp"
#include "stb_image.h"

bool RectangularCuboid::initialized = false;
int RectangularCuboid::instanceCount = 0;
GLuint RectangularCuboid::VAO = 0;
GLuint RectangularCuboid::VBO = 0;

RectangularCuboid::RectangularCuboid(float width, float height, float depth)
	: shader(new Shader("voxCube.vert", "voxCube.frag")), texture(new Texture("cat.png"))
{
	instanceCount++;
	scale = Vec3(width, height, depth);
	rot = Vec3(0, 0, 0);
	pos = Vec3(0, 0, 0);
	initialRot = Vec3(0, 0, 0);
	debug = false;
	if (!initialized)
	{
		initialize();
	}
   	texture = new Texture("cat.png");
	shouldUpdateMats = true;
}

RectangularCuboid::RectangularCuboid( void ) : RectangularCuboid(1, 1, 1){}

void RectangularCuboid::initialize()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
  
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

RectangularCuboid::~RectangularCuboid( void ) 
{
	instanceCount--;
	if (instanceCount == 0)
	{
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glInvalidateBufferData(VBO);
		initialized = false;
	}
	delete shader;
	delete texture;
}

void RectangularCuboid::setMat(Matrix newMat, int type)
{
	if (type == TRANS_MAT)
		transMat = newMat;
	if (type == ROT_MAT)
		rotMat = newMat;
	if (type == SCALE_MAT)
		scaleMat = newMat;
	shouldUpdateMats = true;
}

void RectangularCuboid::draw(Matrix* viewMat)
{
	if (shouldUpdateMats)
		updateMatrixes();
	//Matrix precalcMat = projMat * viewMat * modelMat;
	Matrix precalcMat = projMat * *viewMat;
	precalcMat *= modelMat;
	shader->use();
    glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "proj"), 1, GL_TRUE, projMat.exportForGL());
    glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "view"), 1, GL_TRUE, viewMat->exportForGL());
    glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "model"), 1, GL_TRUE, modelMat.exportForGL());
    glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "precalcMat"), 1, GL_TRUE, precalcMat.exportForGL());
	glBindTexture(GL_TEXTURE_2D, texture->getID());
	glUniform1i(glGetUniformLocation(shader->getID(), "texture0"), 0);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

Matrix RectangularCuboid::getMatrixForOutline()
{
	return myMat * Matrix::createScaleMatrix(scale);
}

Matrix RectangularCuboid::getModelMat()
{
	return modelMat;
}

void RectangularCuboid::setShader(Shader* newShader)
{
	delete shader;
	shader = newShader;
}

void RectangularCuboid::setPos(Vec3 newPos)
{
	pos = newPos;
	shouldUpdateMats = true;
}

void RectangularCuboid::setRot(Vec3 newRot)
{
	rot = newRot;
	shouldUpdateMats = true;
}

void RectangularCuboid::setScale(Vec3 newScale)
{
	scale = newScale;
	shouldUpdateMats = true;
}

//Debug code
void RectangularCuboid::updateMatrixes()
{
	rotMat = Matrix();
	if (rot.x + initialRot.x != 0)
		rotMat = Matrix::createRotationMatrix(Matrix::RotationDirection::X, rot.x + initialRot.x );
	if (rot.y + initialRot.y != 0)
		rotMat *= Matrix::createRotationMatrix(Matrix::RotationDirection::Y, rot.y + initialRot.y );
	if (rot.z + initialRot.z != 0)
		rotMat *= Matrix::createRotationMatrix(Matrix::RotationDirection::Z, rot.z + initialRot.z );
	myMat = Matrix::createTranslationMatrix(pos) * rotMat;// need this
	if (debug)
	{
		std::cout << "myMat after rot" << std::endl;
		myMat.print();
	}
	Matrix mySelfAnchor = Matrix::createTranslationMatrix(selfAnchor * scale * -0.5);
	myMat *= mySelfAnchor;
	if (debug)
	{
		std::cout << "myselfanchor" << std::endl;
		Matrix::createTranslationMatrix(selfAnchor * scale * 0.5).print();
		std::cout << "myMat final after self anchor" << std::endl;
		myMat.print();
	}
	scaleMat = Matrix::createScaleMatrix(scale);
	modelMat = myMat * scaleMat;
	if (debug)
	{
		std::cout << "modelMal" << std::endl;
		modelMat.print();
	}

	shouldUpdateMats = false;
}
