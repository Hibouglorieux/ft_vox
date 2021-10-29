/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Camera.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/02 01:10:32 by nathan            #+#    #+#             */
/*   Updated: 2021/10/22 21:59:06 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef CAMERA_CLASS_H
# define CAMERA_CLASS_H
#include "Matrix.hpp"
#include "appWindow.hpp"
#include "Vec3.hpp"

#define Y_MOVE_SPEED 5.0f

class Camera {
public:
	Camera(void);
	Camera(float x, float y, float z);
	Camera(Vec3 translation);
	void reset();
	~Camera(void) {};
	void move(bool forward, bool backward, bool right, bool left, float speedFactor);
	void moveUp(float distance = Y_MOVE_SPEED);
	void moveDown(float distance = Y_MOVE_SPEED);
	void rotate(double x, double y);
	Vec3 getPos() const;// TODO maybe test with a matrix.getTranslationVector instead for lookAt
	Matrix getMatrix();
	std::pair<Vec3, Vec3> unProject(float mouseX, float mouseY, Matrix projMat);
	Vec3 unProjectToOrigin(float mouseX, float mouseY, Matrix projMat);

private:
	Vec3 getDirection() const;// meant to convert degree to rad
	void actualizeView();
	Matrix view;
	Vec3 dir;
	Vec3 pos;
};

#endif
