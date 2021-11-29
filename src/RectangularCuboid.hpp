/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RectangularCuboid.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/08 17:52:07 by nathan            #+#    #+#             */
/*   Updated: 2021/11/29 16:45:49 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef RECTANGULARCUBOID_CLASS_H
# define RECTANGULARCUBOID_CLASS_H
#include "Object.hpp"
#include "Texture.hpp"

class RectangularCuboid : public Object {
public:
	static void initialize();
//	static void draw(Vec3& pos, Shader* shader, Texture* texture); unusued, drawInstance is called instead
	static void drawInstance(Shader* shader, GLint *instanceTypes,
			GLfloat *instanceTransforms, unsigned int count);
	static void	clear();

private:
	static bool initialized;
    static GLuint VAO, VBO, transformBuffer, typeBuffer;
};

#endif
