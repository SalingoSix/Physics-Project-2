#ifndef _HG_cBulletSphereShape_
#define _HG_cBulletSphereShape_

#include <glm/vec3.hpp>
#include <iShape.h>
#include <btBulletDynamicsCommon.h>

namespace nPhysics
{
	class cBulletSphereShape : public iShape
	{
	public:
		cBulletSphereShape(float radius);

		virtual ~cBulletSphereShape();

		virtual bool sphereGetRadius(float &radius);

		btCollisionShape* getBulletShape();

	private:
		float mRadius;
		btCollisionShape* bulletSphere;
	};
}

#endif