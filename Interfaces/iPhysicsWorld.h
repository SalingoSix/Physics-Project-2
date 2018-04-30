#ifndef _HG_iPhysicsWorld_
#define _HG_iPhysicsWorld_

#include "iRigidBody.h"

namespace nPhysics
{
	class iPhysicsWorld
	{
	public:
		virtual ~iPhysicsWorld() {}

		virtual void TimeStep(float deltaTime) = 0;

		virtual void AddRigidBody(iRigidBody* newBody) = 0;
		virtual void RemoveRigidBody(iRigidBody* oldBody) = 0;
	};
}


#endif