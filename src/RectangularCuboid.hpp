/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RectangularCuboid.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/08 17:52:07 by nathan            #+#    #+#             */
/*   Updated: 2021/12/17 21:40:08 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef RECTANGULARCUBOID_CLASS_H
# define RECTANGULARCUBOID_CLASS_H
#include "Object.hpp"
#include "Texture.hpp"

#define BACK_FACE 0
#define FRONT_FACE 6
#define LEFT_FACE 12
#define RIGHT_FACE 18
#define BOTTOM_FACE 24
#define TOP_FACE 30

#define BACK_NEIGHBOUR 0b1
#define FRONT_NEIGHBOUR 0b10
#define LEFT_NEIGHBOUR 0b100
#define RIGHT_NEIGHBOUR 0b1000
#define BOTTOM_NEIGHBOUR 0b10000
#define UP_NEIGHBOUR 0b100000

class RectangularCuboid : public Object {
public:
	static void initialize();
	static void draw(Vec3& pos, Shader* shader, Texture* texture);// unusued, drawInstance is called instead
	static void drawInstance(Shader* shader, GLuint positionVBO, GLuint typeVBO, unsigned int count);
	static void drawFace(Shader* shader, GLuint positionVBO, GLuint typeVBO, unsigned int count, const std::vector<char>& visibleFaces);
	static void drawFaceInstance(Shader* shader, GLuint positionVBO, GLuint typeVBO, unsigned int count, GLuint facesVBO);
	static void drawQuad(Shader* shader, GLuint positionVBO, GLuint typeVBO);
	static void	clear();

private:
	static bool initialized;
    static GLuint VAO, VBO, texturePosBuf;
};

class Quad : public Object {
	public:
		static void initialize(Vec3 bottomLeft, Vec3 topLeft, Vec3 topRight, Vec3 bottomRight, int _width, int _height);
		static void drawQuad(Shader* shader);
	private:
		static GLuint VAO, VBO, texturePosBuf;
		static int width, height;
		static float vertices[];
};

#endif
