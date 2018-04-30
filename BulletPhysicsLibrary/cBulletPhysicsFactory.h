#ifndef _HG_cBulletPhysicsFactory_
#define _HG_cBulletPhysicsFactory_

#include <iPhysicsFactory.h>
#include <btBulletDynamicsCommon.h>

namespace nPhysics
{
	class cBulletPhysicsFactory : public iPhysicsFactory
	{
		virtual ~cBulletPhysicsFactory();

		virtual iPhysicsWorld* CreateWorld();

		virtual iRigidBody* CreateRigidBody(const sRigidBodyDesc &bodyDesc, iShape* shape);

		virtual iShape* CreateSphere(float radius);
		virtual iShape* CreatePlane(const glm::vec3 cornerA, const glm::vec3 cornerB);
	};
}

#endif