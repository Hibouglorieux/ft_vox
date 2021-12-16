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
#include <GL/glew.h>

#define Y_MOVE_SPEED 5.0f

class Plan {
	public:
		// unit vector
		Vec3	normal = Vec3(0.f, 1.f, 0.f);

		// distance from origin to the nearest point in the plan
		float	distance = 0.f;

		Plan() = default;

		Plan(const Vec3& p1, const Vec3& norm) : normal(norm.getNormalized()), distance(normal.dot(p1))
		{}

		float getSignedDistanceToPlan(const Vec3& point) const
		{
			return normal.dot(point) - distance;
		}

};

struct Frustum
{
	Plan topFace;
	Plan bottomFace;

	Plan rightFace;
	Plan leftFace;

	Plan farFace;
	Plan nearFace;
};

class Camera {
public:

    Vec3 Front;
    Vec3 Up;
    Vec3 Right;
    Vec3 WorldUp;

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
	Vec3 getDirection() const;// meant to convert degree to rad
	Frustum getFrustum() const;
	void updateFrustum(bool blocFrustum);

private:
	void actualizeView();
	Matrix view;
	Vec3 dir;
	Vec3 pos;

	Frustum frustum;
};

#endif
