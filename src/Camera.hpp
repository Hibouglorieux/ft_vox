/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Camera.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/02 01:10:32 by nathan            #+#    #+#             */
/*   Updated: 2021/01/09 03:46:32 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef CAMERA_CLASS_H
# define CAMERA_CLASS_H
#include "Matrix.hpp"
#include "appWindow.hpp"
#include "Vec3.hpp"
#include <utility>

#define Y_MOVE_SPEED 0.2f

class Camera {
public:
	Camera(void);
	Camera(float x, float y, float z);
	Camera(Vec3 translation);
	void reset();
	void lookAt(Vec3 target = {0, 0.0f, 0});
	void freeMovement();
	~Camera(void) {};
	void move(bool forward, bool backward, bool right, bool left, float speedFactor);
	void scroll(double factor);
	void moveUp(float distance = Y_MOVE_SPEED);
	void moveDown(float distance = Y_MOVE_SPEED);
	void rotate(double x, double y);
	Vec3 getDirection() const;
	Vec3 getPos() const;// TODO maybe test with a matrix.getTranslationVector instead for lookAt
	Matrix getMatrix() const;
	std::pair<Vec3, Vec3> unProject(float mouseX, float mouseY, Matrix projMat);
	Vec3 unProjectToOrigin(float mouseX, float mouseY, Matrix projMat);
private:
	bool hasTarget;
	Vec3 target;
	void actualizeView();
	Matrix view;
	Matrix rotMat;
	Vec3 dir;
	Vec3 pos;
};

#endif
