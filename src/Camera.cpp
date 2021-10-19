/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Camera.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/02 01:10:29 by nathan            #+#    #+#             */
/*   Updated: 2021/10/19 14:58:20 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Camera.hpp"
#include "Utilities.h"
#include <utility>

#define X_ROTATION_SPEED 1
#define Y_ROTATION_SPEED 1

Camera::Camera() : Camera(Vec3(0, 20, 0))
{
	dir.x = 0.0f;//89.0f;// TODO tmp to view map generation from ahead to view map generation from ahead
	actualizeView();
}

Camera::Camera(float x, float y, float z) : Camera(Vec3(x, y, z))
{
}

Camera::Camera(Vec3 position)
{
	this->pos = position;
	dir = {0, 0, 0};
	actualizeView();
}

Vec3 Camera::getPos() const
{
	return pos;
}

void Camera::reset()
{
	dir = {0, 0, 0};
	pos = {0, 4, 10};
	actualizeView();
}

Matrix Camera::getMatrix() const
{
	return view;
}

void Camera::freeMovement()
{
	dir = {0, 0, 0};
	pos = {0, 0, 50};
	actualizeView();
}

void Camera::move(bool forward, bool backward, bool right, bool left, float speedFactor)
{
	if (!forward && !backward && !right && !left)
		return;
	Vec3 moveDir = getDirection();
	Vec3 realMovement;
	if (forward)
		realMovement = moveDir;
	if (backward)
		realMovement -= moveDir;
	moveDir.y = 0;
	if (right)
	{
		realMovement += Matrix::createRotationMatrix(Matrix::RotationDirection::Y, 90) * moveDir;
	}
	if (left)
	{
		realMovement += Matrix::createRotationMatrix(Matrix::RotationDirection::Y, -90) * moveDir;
	}
	pos += realMovement.getNormalized() * speedFactor;
	if (realMovement != Vec3::ZERO)
		actualizeView();
}

Vec3 Camera::getDirection() const
{
	Vec3 moveDir;
	moveDir.z = -cos(TO_RAD(dir.y)) * cos(TO_RAD(dir.x));
	moveDir.y = -sin(TO_RAD(dir.x));
	moveDir.x = sin(TO_RAD(dir.y)) * cos(TO_RAD(dir.x));
	moveDir.getNormalized();

	return moveDir;
}

void Camera::moveUp(float distance)
{
	pos.y += distance;
	actualizeView();
}

void Camera::moveDown(float distance)
{
	pos.y -= distance;
	actualizeView();
}

void Camera::rotate(double x, double y)
{
	if (x == 0 && y == 0)
		return;
	dir.y += x;
	dir.x += y;
	if (dir.x > 89.0f)
		dir.x = 89;
	if (dir.x < -89.0f)
		dir.x = -89.0f;
	actualizeView();
}

void Camera::actualizeView()
{
	rotMat = Matrix::createRotationMatrix(Matrix::RotationDirection::X, dir.x);
	rotMat *= Matrix::createRotationMatrix(Matrix::RotationDirection::Y, dir.y);
	this->view = rotMat * Matrix::createTranslationMatrix(-pos);
}

Vec3 Camera::unProjectToOrigin(float mouseX, float mouseY, Matrix projMat)
{
	std::pair<Vec3, Vec3> points = unProject(mouseX, mouseY, projMat);
	Vec3 point1 = std::get<0>(points);
	Vec3 point2 = std::get<1>(points);
	Vec3 direction = point2 - point1;
	direction = direction.getNormalized();
	Vec3 origin(0, 0, 0);

	float result = (origin - point1).dot(direction) / direction.dot(direction);
	return point1 + (direction * result);
	/*
	 *
	 * plane with three points 1, 0, 0
	 * 						   0, 1, 0
	 * 						   0, 0, 0
	 * 	rotate these with camera rotation => plane parallel to camera one
	 *	calculate intersection between line and that plane
	 *
	 *
	 */
}

std::pair<Vec3, Vec3> Camera::unProject(float mouseX, float mouseY, Matrix projMat)
{
	//https://doxygen.reactos.org/d2/d0d/project_8c_source.html
	Matrix finalMat;
	Vec3 point1, point2;
	int width, height;

	appWindow::getWindowSize(&width, &height);
	mouseY = height - mouseY;

	point1 = {mouseX, mouseY, 0.f};
	point2 = {mouseX, mouseY, 1.f};


	point1.x = point1.x / (width * 0.5f) - 1.0f;
	point2.x = point2.x / (width * 0.5f) - 1.0f;
	point1.y = point1.y / (height * 0.5f) - 1.0f;
	point2.y = point2.y / (height * 0.5f) - 1.0f;

	finalMat = (projMat * getMatrix()).invert();
	point1 = finalMat * point1;
	point2 = finalMat * point2;

	return std::make_pair(point1, point2);
}
