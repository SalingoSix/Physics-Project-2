#include "cBulletPhysicsFactory.h"
#include "cBulletPhysicsWorld.h"
#include "cBulletRigidBody.h"
#include "cBulletSphereShape.h"
#include "cBulletPlaneShape.h"

namespace nPhysics
{
	cBulletPhysicsFactory::~cBulletPhysicsFactory()
	{

	}

	iPhysicsWorld* cBulletPhysicsFactory::CreateWorld()
	{
		return new cBulletPhysicsWorld();
	}

	iRigidBody* cBulletPhysicsFactory::CreateRigidBody(const sRigidBodyDesc &bodyDesc, iShape* shape)
	{
		return new cBulletRigidBody(bodyDesc, shape);
	}

	iShape* cBulletPhysicsFactory::CreateSphere(float radius)
	{
		return new cBulletSphereShape(radius);
	}

	iShape* cBulletPhysicsFactory::CreatePlane(const glm::vec3 cornerA, const glm::vec3 cornerB)
	{
		return new cBulletPlaneShape(cornerA, cornerB);
	}
}