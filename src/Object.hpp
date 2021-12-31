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

# define BB_VERBOSE false

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
	Vec3 worldPos{ 0.f, 0.f, 0.f };

	AABB(const Vec3& min, const Vec3& max)
		: BoundingVolume{}, center{(max - min) * 0.5f}, extents{(max - min) *  0.5f}, worldPos{(min + extents)}
	{
		/*min.print();
		max.print();
		center.print();*/
	}

	AABB(const Vec3& inCenter, float iI, float iJ, float iK, Vec3 worldPos)
		: BoundingVolume{}, center{ inCenter }, extents{ iI, iJ, iK }, worldPos{worldPos}
	{}

	//see https://gdbooks.gitbooks.io/3dcollisions/content/Chapter2/static_aabb_plan.html
	bool isOnOrForwardPlan(const Plan& plan) const final
	{
		// Compute the projection interval radius of b onto L(t) = b.c + t * p.n
		const float r = extents.x * std::abs(plan.normal.x) + extents.y * std::abs(plan.normal.y) +
			extents.z * std::abs(plan.normal.z);

		if (BB_VERBOSE && -r >= plan.getSignedDistanceToPlan(center))
		{
			printf("WorldPos : ");
			worldPos.print();
			printf("Center : ");
			center.print();
			printf("Normal : ");
			plan.normal.print();
			printf("R : %f | Signed dist to plan : %f\n", -r, plan.getSignedDistanceToPlan(center));
		}

		return -r <= plan.getSignedDistanceToPlan(center);
	}

	bool isOnFrustum(const Frustum& camFrustum) const
	{
		// TODO : Update scale correctly
		//Get global scale thanks to our transform
		//const Vec3 globalCenter{ transform.getModelMatrix() * glm::vec4(center, 1.f) };
		const Vec3 globalCenter = worldPos;

		// Scaled orientation
		const Vec3 right = Vec3(1, 0, 0) * extents.x;//transform.getRight() * extents.x;
		const Vec3 up = Vec3(0, 1, 0) * extents.y;//transform.getUp() * extents.y;
		const Vec3 forward = Vec3(0, 0, 1) * extents.z;//transform.getForward() * extents.z;

		/*printf("STUFF\n");
		extents.print();
		center.print();
		right.print();
		up.print();
		forward.print();*/

		const float newIi = std::abs(Vec3(1.f, 0.f, 0.f).dot(right)) +
			std::abs(Vec3(1.f, 0.f, 0.f).dot(up)) +
			std::abs(Vec3(1.f, 0.f, 0.f).dot(forward));

		const float newIj = std::abs(Vec3(0.f, 1.f, 0.f).dot(right)) +
			std::abs(Vec3(0.f, 1.f, 0.f).dot(up)) +
			std::abs(Vec3(0.f, 1.f, 0.f).dot(forward));

		const float newIk = std::abs(Vec3(0.f, 0.f, 1.f).dot(right)) +
			std::abs(Vec3(0.f, 0.f, 1.f).dot(up)) +
			std::abs(Vec3(0.f, 0.f, 1.f).dot(forward));
		/*const float newIi = extents.x;
		const float newIj = extents.y;
		const float newIk = extents.z;*/

		//printf("%f %f %f\n", newIi, newIj, newIk);

		const AABB globalAABB(globalCenter, newIi, newIj, newIk, worldPos);

		/*printf("Global AABB variables : \n");
		globalAABB.center.print();
		globalAABB.extents.print();
		globalAABB.worldPos.print();
		printf("\n");*/

		bool bleft, bright, btop, bbottom, bnear, bfar = false;
		if (BB_VERBOSE)
		{
			bleft = globalAABB.isOnOrForwardPlan(camFrustum.leftFace);
			//if (bleft) printf("Left  face\n");
			bright = globalAABB.isOnOrForwardPlan(camFrustum.rightFace);
			//if (bright) printf("Right face\n");
			btop = globalAABB.isOnOrForwardPlan(camFrustum.rightFace);
			//if (btop) printf("Top face\n");
			bbottom = globalAABB.isOnOrForwardPlan(camFrustum.rightFace);
			//if (bbottom) printf("Bottom face\n");
			bnear = globalAABB.isOnOrForwardPlan(camFrustum.rightFace);
			//if (bnear) printf("Near face\n");
			bfar = globalAABB.isOnOrForwardPlan(camFrustum.rightFace);
			//if (bfar) printf("Far face\n");
			//printf("\n");
		}
		else
		{
			bleft = globalAABB.isOnOrForwardPlan(camFrustum.leftFace);
			bright = globalAABB.isOnOrForwardPlan(camFrustum.rightFace);
			btop = globalAABB.isOnOrForwardPlan(camFrustum.rightFace);
			bbottom = globalAABB.isOnOrForwardPlan(camFrustum.rightFace);
			bnear = globalAABB.isOnOrForwardPlan(camFrustum.rightFace);
			bfar = globalAABB.isOnOrForwardPlan(camFrustum.rightFace);
		}

		return (bleft && bright && btop && bbottom && bnear && bfar);
	};
};

#endif
