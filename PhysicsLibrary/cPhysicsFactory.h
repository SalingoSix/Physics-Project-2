#ifndef _HG_cPhysicsFactory_
#define _HG_cPhysicsFactory_

#include <iPhysicsFactory.h>

namespace nPhysics
{
	class cPhysicsFactory : public iPhysicsFactory
	{
		virtual ~cPhysicsFactory();

		virtual iPhysicsWorld* CreateWorld();

		virtual iRigidBody* CreateRigidBody(const sRigidBodyDesc &bodyDesc, iShape* shape);

		virtual iShape* CreateSphere(float radius);
		virtual iShape* CreatePlane(const glm::vec3 cornerA, const glm::vec3 cornerB);
	};
}

#endif