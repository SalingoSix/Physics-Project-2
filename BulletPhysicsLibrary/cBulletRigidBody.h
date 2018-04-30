#ifndef _HG_cBulletRigidBody_
#define _HG_cBulletRigidBody_

#include <iRigidBody.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <btBulletDynamicsCommon.h>

#include "cBulletPlaneShape.h"
#include "cBulletSphereShape.h"

namespace nPhysics
{
	class cBulletPhysicsWorld;
	class cBulletRigidBody : public iRigidBody
	{
	public:
		cBulletRigidBody(const sRigidBodyDesc desc, iShape* shape);
		virtual ~cBulletRigidBody();

		virtual iShape* getShape();

		virtual void getTransform(glm::mat4 &transform);
		virtual void getPosition(glm::vec3 &position);
		virtual void getRotataion(glm::vec3 &rotation);
		virtual void getLastSafePos(glm::vec3 &safe);
		virtual void applyForce(glm::vec3 force);

	private:
		friend class cBulletPhysicsWorld;

		iShape* mShape;
		glm::vec3 mPosition;
		glm::vec3 lastSafePos;
		glm::vec3 mVelocity;
		glm::quat mRotation;
		glm::vec3 mAccel;
		glm::vec3 appliedForce;
		float mMass;

		btRigidBody::btRigidBodyConstructionInfo* info;
		btRigidBody* bulletRigidBody;
	};
}

#endif