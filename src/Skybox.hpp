/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Skybox.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/20 18:53:39 by nathan            #+#    #+#             */
/*   Updated: 2021/10/21 11:17:37 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef SKYBOX_CLASS_H
# define SKYBOX_CLASS_H

#include "Texture.hpp"
#include "Shader.hpp"
#include "Matrix.hpp"
#include "RectangularCuboid.hpp"
#include "VoxelGenerator.hpp"

class Skybox {
public:
	static void initialize();
	static void draw(Matrix& precalculatedMat);
	static void clear();
private:
	static Texture* texture;
	static Shader* shader;
    static GLuint VAO, VBO;

	static float ToDELETEClamp(float x, float low, float high);
	static float ToDELETESmoothStep(float a, float b, float x, float c1, float c2);
};

#endif
