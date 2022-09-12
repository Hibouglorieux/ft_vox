/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Skybox.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/20 18:53:39 by nathan            #+#    #+#             */
/*   Updated: 2022/09/12 16:31:12 by nallani          ###   ########.fr       */
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
	static void initialize(float freq = 1.0f, float amp = 1.0f, int octaves = 1, int depth = 0);
	static void draw(Matrix& precalcMat);
	static void clear();
	static void changeLight(bool light);

	static Vec3 playerPos;
private:
	static Texture* texture;
	static Shader* shader;
    static GLuint VAO, VBO;

	static float ToDELETEClamp(float x, float low, float high);
	static float ToDELETESmoothStep(float a, float b, float x, float c1, float c2);
};

#endif
