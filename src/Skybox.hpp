/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Skybox.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/20 18:53:39 by nathan            #+#    #+#             */
/*   Updated: 2021/10/20 19:24:40 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef SKYBOX_CLASS_H
# define SKYBOX_CLASS_H

#include "Texture.hpp"
#include "Shader.hpp"
#include "Matrix.hpp"
#include "RectangularCuboid.hpp"

class Skybox {
public:
	static void initialize();
	static void draw(Matrix& precalculatedMat);
	static void clear();
private:
	static Texture* texture;
	static Shader* shader;
    static GLuint VAO, VBO;

};

#endif
