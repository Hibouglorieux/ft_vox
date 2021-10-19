/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RectangularCuboid.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/08 17:52:07 by nathan            #+#    #+#             */
/*   Updated: 2021/10/18 17:46:06 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef RECTANGULARCUBOID_CLASS_H
# define RECTANGULARCUBOID_CLASS_H
#include "Object.hpp"
#include "Texture.hpp"

class RectangularCuboid : public Object {
public:
	static void initialize();
	static void draw(Vec3& pos, Shader* shader, Texture* texture);
	static void drawInstance(Vec3& pos, Shader* shader, Texture* texture,
			GLfloat *instanceTransforms, unsigned int count);
	static void	clear();

private:
	static bool initialized;
    static GLuint VAO, VBO, transformBuffer;
};

#endif
