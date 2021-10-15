/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Object.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/08 19:05:09 by nathan            #+#    #+#             */
/*   Updated: 2020/10/22 12:27:07 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef OBJECT_CLASS_H
# define OBJECT_CLASS_H

#include "Utilities.h"
#include "Matrix.hpp"
#include "Shader.hpp"
#include <tuple>
#include <functional>
#include <vector>
#include <algorithm>

class Object {
public:
	Object(void);
	virtual void draw(Matrix* viewMat) = 0;
	virtual void draw(Matrix* viewMat, Shader* specialEffect, std::vector<std::tuple<std::function<void(GLint, GLsizei, const GLfloat*)>, std::string, const GLfloat*>> shaderData) = 0;
	virtual void drawChildren(Matrix* viewMat);
	virtual void drawChildren(Matrix* viewMat, Shader* specialEffect, std::vector<std::tuple<std::function<void(GLint, GLsizei, const GLfloat*)>, std::string, const GLfloat*>> shaderData);
	virtual ~Object(void);
	virtual Vec3 getPos() const = 0;
	static void setProjMat(Matrix projMat);
	void addChild(Object* newchild);
	void removeChild(Object* child);
	void setParent(Object* parent);
	void setID(std::string newID);
	std::string getID() const;
	virtual void onNewParent() {};
	Object* getParent();
	std::vector<Object*> getChildren() {return children;}
protected:
	std::string ID;
	std::vector<Object*> children;
	Object* parent;
	static Matrix projMat;
};

#endif
