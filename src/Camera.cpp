/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Camera.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/02 01:10:29 by nathan            #+#    #+#             */
/*   Updated: 2021/12/10 17:33:57 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Camera.hpp"
#include "ft_vox.h"
#include "Utilities.h"
#include <utility>

#define X_ROTATION_SPEED 1
#define Y_ROTATION_SPEED 1
//#define DEFAULT_CAMERA_POS Vec3(0.f, 90.f, 0.f)
//#define DEFAULT_CAMERA_ROT Vec3(0.f, 90.f, 0.f)
#define DEFAULT_CAMERA_POS Vec3(88.08f, 43.01f, -69.01f)
#define DEFAULT_CAMERA_ROT Vec3(0.f, 0.f, 0.f)
//#define DEFAULT_CAMERA_POS Vec3(-43.28f, 41.15f, -44.17f)
//#define DEFAULT_CAMERA_ROT Vec3(90.f, 90.f, 0.f)
//#define DEFAULT_CAMERA_POS Vec3(-43.28f, 260.15f, -44.17f)

Camera::Camera() : Camera(DEFAULT_CAMERA_POS)
{
}

Camera::Camera(float x, float y, float z) : Camera(Vec3(x, y, z))
{
}

Camera::Camera(Vec3 position)
{
	pos = position;
	dir = DEFAULT_CAMERA_ROT;// TODO tmp to view map generation from ahead

	WorldUp = Vec3(0, 1, 0);
	Up = Vec3(0, 1, 0); // TODO : Update to use default cam rot
	Right = Vec3(1, 0, 0); // TODO : Update to use default cam rot

	Front = getDirection();
	Right = Up.cross(Front).getNormalized();
	Up = Front.cross(Right).getNormalized();
	updateFrustum(false);
}

void Camera::updateFrustum(bool blocFrustum)
{
	(void)blocFrustum;
	float	halfVSide, halfHSide = 0;
    Vec3	frontMultFar;
	int		height, width;

	appWindow::getWindowSize(&width, &height);
	halfVSide = -FAR * tanf(FOV_RAD * 0.60f);

	halfHSide = (halfVSide * ((float)width / (float)height));
	frontMultFar = getDirection() * FAR;

	frustum.nearFace 	=	{ pos + Front * NEAR, Front 										};
	frustum.farFace 	=	{ pos + frontMultFar, -Front 										};
	frustum.rightFace 	=	{ pos				, Up.cross(frontMultFar + Right * halfHSide) 	};
	frustum.leftFace 	=	{ pos				, (frontMultFar - Right * halfHSide).cross(Up) 	};
	frustum.topFace 	=	{ pos				, Right.cross(frontMultFar - Up * halfVSide) 	};
	frustum.bottomFace 	= 	{ pos				, (frontMultFar + Up * halfVSide).cross(Right) 	};
}

Frustum Camera::getFrustum() const
{
	return frustum;
}

Vec3 Camera::getPos() const
{
	return pos;
}

void Camera::reset()
{
	pos = DEFAULT_CAMERA_POS;
	dir = DEFAULT_CAMERA_ROT;
}

Matrix Camera::getMatrix()
{
	actualizeView();

	updateFrustum(false);
	return view;
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
	//printf("Position : %3f %3f %3f | %3f %3f %3f\n", pos.x, pos.y, pos.z, dir.x, dir.y, dir.z);
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
}

void Camera::moveDown(float distance)
{
	pos.y -= distance;
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

	Front = getDirection();
	Right = Up.cross(Front).getNormalized();
	Up = Front.cross(Right).getNormalized();
}

void Camera::actualizeView()
{
	Matrix rotMat = Matrix::createRotationMatrix(Matrix::RotationDirection::X, dir.x);
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
