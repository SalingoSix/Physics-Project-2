#include "cBulletSphereShape.h"

namespace nPhysics
{
	cBulletSphereShape::cBulletSphereShape(float radius)
		: iShape(SHAPE_SPHERE)
	{
		bulletSphere = new btSphereShape(radius);
		mRadius = radius;
	}

	cBulletSphereShape::~cBulletSphereShape()
	{

	}

	bool cBulletSphereShape::sphereGetRadius(float &radius)
	{
		btSphereShape* castSphere = (btSphereShape*)bulletSphere;
		btScalar theRadius = castSphere->getRadius();
		radius = theRadius;
		return true;
	}

	btCollisionShape* cBulletSphereShape::getBulletShape()
	{
		return this->bulletSphere;
	}
}