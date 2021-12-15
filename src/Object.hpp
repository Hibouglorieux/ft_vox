/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Object.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/08 19:05:09 by nathan            #+#    #+#             */
/*   Updated: 2021/10/19 22:33:04 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef OBJECT_CLASS_H
# define OBJECT_CLASS_H

#include "Matrix.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#define SCREEN_WIDTH 800.0f
#define SCREEN_HEIGHT 600.0f //TODO change that to have the real window size

class Object {
public:
	Object(void);
	virtual void	draw(Shader* shader) = 0;
	virtual ~Object(void);
	static void setProjMat(Matrix projMat);
	static Matrix getProjMat(){return projMat;} // TODO might need to remove
protected:
	static Matrix projMat;
};

struct BoundingVolume
{
	//virtual bool isOnFrustum(const Frustum& camFrustum, const Transform& transform) const = 0;

	virtual bool isOnOrForwardPlan(const Plan& plan) const = 0;

	bool isOnFrustum(const Frustum& camFrustum) const
	{
		return (isOnOrForwardPlan(camFrustum.leftFace) &&
			isOnOrForwardPlan(camFrustum.rightFace) &&
			isOnOrForwardPlan(camFrustum.topFace) &&
			isOnOrForwardPlan(camFrustum.bottomFace) &&
			isOnOrForwardPlan(camFrustum.nearFace) &&
			isOnOrForwardPlan(camFrustum.farFace));
	};
};

struct AABB : public BoundingVolume
{
	Vec3 center{ 0.f, 0.f, 0.f };
	Vec3 extents{ 0.f, 0.f, 0.f };

	AABB(const Vec3& min, const Vec3& max)
		: BoundingVolume{}, center{(min)}, extents{ max.x - center.x, max.y - center.y, max.z - center.z }
	{}

	AABB(const Vec3& inCenter, float iI, float iJ, float iK)
		: BoundingVolume{}, center{ inCenter }, extents{ iI, iJ, iK }
	{}

	//see https://gdbooks.gitbooks.io/3dcollisions/content/Chapter2/static_aabb_plan.html
	bool isOnOrForwardPlan(const Plan& plan) const final
	{
		// Compute the projection interval radius of b onto L(t) = b.c + t * p.n
		const float r = extents.x * std::abs(plan.normal.x) + extents.y * std::abs(plan.normal.y) +
			extents.z * std::abs(plan.normal.z);

		/*printf("Center : ");
		center.print();
		printf("R : %f | Signed dist to plan : %f\n", -r, plan.getSignedDistanceToPlan(center));*/

		return -r <= plan.getSignedDistanceToPlan(center);
	}

	bool isOnFrustum(const Frustum& camFrustum) const
	{
		// TODO : Update scale correctly
		//Get global scale thanks to our transform
		//const Vec3 globalCenter{ transform.getModelMatrix() * glm::vec4(center, 1.f) };
		const Vec3 globalCenter = Vec3(center);

		// Scaled orientation
		const Vec3 right = Vec3(1, 0, 0) * extents.x;//transform.getRight() * extents.x;
		const Vec3 up = Vec3(0, 1, 0) * extents.y;//transform.getUp() * extents.y;
		const Vec3 forward = Vec3(0, 0, 1) * extents.z;//transform.getForward() * extents.z;

		const float newIi = std::abs(Vec3(1.f, 0.f, 0.f).dot(right)) +
			std::abs(Vec3(1.f, 0.f, 0.f).dot(up)) +
			std::abs(Vec3(1.f, 0.f, 0.f).dot(forward));

		const float newIj = std::abs(Vec3(0.f, 1.f, 0.f).dot(right)) +
			std::abs(Vec3(0.f, 1.f, 0.f).dot(up)) +
			std::abs(Vec3(0.f, 1.f, 0.f).dot(forward));

		const float newIk = std::abs(Vec3(0.f, 0.f, 1.f).dot(right)) +
			std::abs(Vec3(0.f, 0.f, 1.f).dot(up)) +
			std::abs(Vec3(0.f, 0.f, 1.f).dot(forward));

		const AABB globalAABB(globalCenter, newIi, newIj, newIk);

		return ((globalAABB.isOnOrForwardPlan(camFrustum.leftFace) ||
			globalAABB.isOnOrForwardPlan(camFrustum.rightFace) ||
			globalAABB.isOnOrForwardPlan(camFrustum.topFace) ||
			globalAABB.isOnOrForwardPlan(camFrustum.bottomFace)) && (
			globalAABB.isOnOrForwardPlan(camFrustum.nearFace) ||
			globalAABB.isOnOrForwardPlan(camFrustum.farFace)));
	};
};

#endif
