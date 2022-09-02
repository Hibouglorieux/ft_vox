/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Faces.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/02 17:35:56 by nallani           #+#    #+#             */
/*   Updated: 2022/09/02 22:37:19 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef FACES_CLASS_H
# define FACES_CLASS_H

#include <GL/glew.h>
#include "Shader.hpp"

struct VerticeArrayData
{
public:
	float data[30];
};

class Face
{
public:
	Face(void){};
	virtual ~Face(void){};

	void Initialize();
	void Clear();

	//draw()
protected:

	static void drawCommon(Shader* shader, GLuint positionVBO, GLuint TextureVBO, unsigned int count);

	virtual bool getInitialized() = 0;
	virtual void setInitialized(bool value) = 0;
	virtual const VerticeArrayData getVerticesData() = 0;

	virtual void SetVAO(GLuint newVAO) = 0;
	virtual GLuint GetVAO() = 0;

	virtual void SetVBO(GLuint newVBO) = 0;
	virtual GLuint GetVBO() = 0;
};

class FrontFace : public Face{
public:
	FrontFace(void) {Face::Initialize();};
	virtual ~FrontFace(void) {Face::Clear();};
	static void draw(Shader* shader, GLuint positionVBO, GLuint TextureVBO, unsigned int count);
protected:
	virtual bool getInitialized() override {return initialized;} ;
	virtual void setInitialized(bool value) override {initialized = value;} ;
	virtual const VerticeArrayData getVerticesData() override {
		return VerticeArrayData{
		{
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
		}
		};} ;

	virtual void SetVAO(GLuint newVAO) override {VAO = newVAO;} ;
	virtual GLuint GetVAO() override {return VAO;} ;

	virtual void SetVBO(GLuint newVBO) override {VBO = newVBO;} ;
	virtual GLuint GetVBO() override {return VBO;} ;
private:
	static bool initialized;
    static GLuint VAO, VBO;
};

class BackFace : public Face{
public:
	BackFace(void) {Face::Initialize();};
	virtual ~BackFace(void) {Face::Clear();};
	static void draw(Shader* shader, GLuint positionVBO, GLuint TextureVBO, unsigned int count);
protected:
	virtual bool getInitialized() override {return initialized;} ;
	virtual void setInitialized(bool value) override {initialized = value;} ;
	virtual const VerticeArrayData getVerticesData() override {
		return VerticeArrayData{
		{
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right         
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
		}
		};} ;

	virtual void SetVAO(GLuint newVAO) override {VAO = newVAO;} ;
	virtual GLuint GetVAO() override {return VAO;} ;

	virtual void SetVBO(GLuint newVBO) override {VBO = newVBO;} ;
	virtual GLuint GetVBO() override {return VBO;} ;
private:
	static bool initialized;
    static GLuint VAO, VBO;
};

class LeftFace : public Face{
public:
	LeftFace(void) {Face::Initialize();};
	virtual ~LeftFace(void) {Face::Clear();};
	static void draw(Shader* shader, GLuint positionVBO, GLuint TextureVBO, unsigned int count);
protected:
	virtual bool getInitialized() override {return initialized;} ;
	virtual void setInitialized(bool value) override {initialized = value;} ;
	virtual const VerticeArrayData getVerticesData() override {
		return VerticeArrayData{
		{
										  // Left face
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
		}
		};} ;

	virtual void SetVAO(GLuint newVAO) override {VAO = newVAO;} ;
	virtual GLuint GetVAO() override {return VAO;} ;

	virtual void SetVBO(GLuint newVBO) override {VBO = newVBO;} ;
	virtual GLuint GetVBO() override {return VBO;} ;
private:
	static bool initialized;
    static GLuint VAO, VBO;
};

class RightFace : public Face{
public:
	RightFace(void) {Face::Initialize();};
	virtual ~RightFace(void) {Face::Clear();};
	static void draw(Shader* shader, GLuint positionVBO, GLuint TextureVBO, unsigned int count);
protected:
	virtual bool getInitialized() override {return initialized;} ;
	virtual void setInitialized(bool value) override {initialized = value;} ;
	virtual const VerticeArrayData getVerticesData() override {
		return VerticeArrayData{
		{
										  // Right face
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right         
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left     
		}
		};} ;

	virtual void SetVAO(GLuint newVAO) override {VAO = newVAO;} ;
	virtual GLuint GetVAO() override {return VAO;} ;

	virtual void SetVBO(GLuint newVBO) override {VBO = newVBO;} ;
	virtual GLuint GetVBO() override {return VBO;} ;
private:
	static bool initialized;
    static GLuint VAO, VBO;
};

class BottomFace : public Face{
public:
	BottomFace(void) {Face::Initialize();};
	virtual ~BottomFace(void) {Face::Clear();};
	static void draw(Shader* shader, GLuint positionVBO, GLuint TextureVBO, unsigned int count);
protected:
	virtual bool getInitialized() override {return initialized;} ;
	virtual void setInitialized(bool value) override {initialized = value;} ;
	virtual const VerticeArrayData getVerticesData() override {
		return VerticeArrayData{
		{
										 // Bottom face
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
		}
		};} ;

	virtual void SetVAO(GLuint newVAO) override {VAO = newVAO;} ;
	virtual GLuint GetVAO() override {return VAO;} ;

	virtual void SetVBO(GLuint newVBO) override {VBO = newVBO;} ;
	virtual GLuint GetVBO() override {return VBO;} ;
private:
	static bool initialized;
    static GLuint VAO, VBO;
};

class TopFace : public Face{
public:
	TopFace(void) {Face::Initialize();};
	virtual ~TopFace(void) {Face::Clear();};
	static void draw(Shader* shader, GLuint positionVBO, GLuint TextureVBO, unsigned int count);
protected:
	virtual bool getInitialized() override {return initialized;} ;
	virtual void setInitialized(bool value) override {initialized = value;} ;
	virtual const VerticeArrayData getVerticesData() override {
		return VerticeArrayData{
		{
										  // Top face
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right     
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  // bottom-left        
		}
		};} ;

	virtual void SetVAO(GLuint newVAO) override {VAO = newVAO;} ;
	virtual GLuint GetVAO() override {return VAO;} ;

	virtual void SetVBO(GLuint newVBO) override {VBO = newVBO;} ;
	virtual GLuint GetVBO() override {return VBO;} ;
private:
	static bool initialized;
    static GLuint VAO, VBO;
};
#endif
