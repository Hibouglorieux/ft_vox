/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RectangularCuboid.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/08 17:52:07 by nathan            #+#    #+#             */
/*   Updated: 2022/09/09 16:46:28 by nallani          ###   ########.fr       */
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
	static void drawFaceInstance(Shader* shader, GLuint allVBO, unsigned int count);
	static void	clear();

private:
	static bool initialized;
    static GLuint VAO, VBO, texturePosBuf;
};

#endif
