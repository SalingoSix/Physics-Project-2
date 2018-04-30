#include "cBulletPlaneShape.h"

#include <glm/geometric.hpp>

namespace nPhysics
{
	cBulletPlaneShape::cBulletPlaneShape(glm::vec3 cornerA, glm::vec3 cornerB)
		: iShape(SHAPE_PLANE)
	{
		btVector3 btNormal;
		btScalar planeConst;
		bottomCorner = cornerA;
		topCorner = cornerB;

		if (topCorner.x == bottomCorner.x)
		{
			normalPlane = 1;
			if (topCorner.x > 0)
			{
				normalVec = glm::vec3(-1.0f, 0.0f, 0.0f);
			}
			else
			{
				normalVec = glm::vec3(1.0f, 0.0f, 0.0f);
			}
			
			planeConst = -15.0f;
		}
		else if (topCorner.y == bottomCorner.y)
		{
			normalPlane = 2;
			normalVec = glm::vec3(0.0f, 1.0f, 0.0f);
			planeConst = topCorner.y;
		}
		else
		{
			normalPlane = 3;
			if (topCorner.z > 0)
			{
				normalVec = glm::vec3(0.0f, 0.0f, -1.0f);
			}
			else
			{
				normalVec = glm::vec3(0.0f, 0.0f, 1.0f);
			}
			planeConst = -15.0f;
		}
		btNormal = toBtVec(normalVec);
		bulletPlane = new btStaticPlaneShape(btNormal, planeConst);
		this->planeConst = glm::dot(topCorner, bottomCorner);
	}

	cBulletPlaneShape::~cBulletPlaneShape()
	{

	}

	int cBulletPlaneShape::planeGetNormal()
	{
		return normalPlane;
	}
	bool cBulletPlaneShape::planeGetBottomCorner(glm::vec3 &corner)
	{
		corner = bottomCorner;
		return true;
	}
	bool cBulletPlaneShape::planeGetTopCorner(glm::vec3 &corner)
	{
		corner = topCorner;
		return true;
	}
	bool cBulletPlaneShape::planeGetNormalVec(glm::vec3 &normal)
	{
		btStaticPlaneShape* castPlane = (btStaticPlaneShape*)bulletPlane;
		btVector3 btNormal = castPlane->getPlaneNormal();
		normal = toGLMVec(btNormal);
		return true;
	}
	bool cBulletPlaneShape::planeGetPlaneConst(float &special)
	{
		btStaticPlaneShape* castPlane = (btStaticPlaneShape*)bulletPlane;
		btScalar btNormal = castPlane->getPlaneConstant();
		special = btNormal;
		return true;
	}

	btCollisionShape* cBulletPlaneShape::getBulletShape()
	{
		return this->bulletPlane;
	}
}