#include "cBulletRigidBody.h"

namespace nPhysics
{
	cBulletRigidBody::cBulletRigidBody(const sRigidBodyDesc desc, iShape* shape)
	{
		/// Create Dynamic Objects
		btTransform startTransform;
		startTransform.setIdentity();

		btScalar mass = desc.Mass;

		bool isDynamic;

		btVector3 localInertia;

		btCollisionShape* colShape = 0;

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		eShapeType theType = shape->getShapeType();
		if (theType == nPhysics::SHAPE_PLANE)
		{
			isDynamic = false;
			mass = 0.0f;
			localInertia = toBtVec(glm::vec3(0, 0, 0));
			cBulletPlaneShape* castShape = (cBulletPlaneShape*)shape;
			colShape = castShape->getBulletShape();
		}
		else if (theType == nPhysics::SHAPE_SPHERE)
		{
			isDynamic = (mass != 0.f);
			localInertia = toBtVec(desc.Velocity);
			cBulletSphereShape* castShape = (cBulletSphereShape*)shape;
			colShape = castShape->getBulletShape();
			if (isDynamic)
				colShape->calculateLocalInertia(mass, localInertia);
		}

		startTransform.setOrigin(toBtVec(desc.Position));


		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
		//info = rbInfo;
		bulletRigidBody = new btRigidBody(rbInfo);

		bulletRigidBody->setRestitution(0.9);

		mShape = shape;
		mPosition = desc.Position;
		lastSafePos = desc.Position;
		mVelocity = desc.Velocity;
		mRotation = desc.Rotation;
		mMass = desc.Mass;
	}

	cBulletRigidBody::~cBulletRigidBody()
	{
		if (bulletRigidBody && bulletRigidBody->getMotionState())
		{
			delete bulletRigidBody->getMotionState();
		}
	}

	iShape* cBulletRigidBody::getShape()
	{
		return mShape;
	}

	void cBulletRigidBody::getTransform(glm::mat4 &transform)
	{
		btTransform worldTrans;
		bulletRigidBody->getMotionState()->getWorldTransform(worldTrans);
		ToGlm(worldTrans, transform);
	}

	void cBulletRigidBody::getPosition(glm::vec3 &position)
	{
		position = toGLMVec(bulletRigidBody->getCenterOfMassPosition());
	}

	void cBulletRigidBody::getRotataion(glm::vec3 &rotation)
	{
		btTransform transform;
		bulletRigidBody->getMotionState()->getWorldTransform(transform);
		rotation = toGLMQuatToVec(transform.getRotation());
	}

	void cBulletRigidBody::getLastSafePos(glm::vec3 &safePos)
	{
		safePos = lastSafePos;
	}

	void cBulletRigidBody::applyForce(glm::vec3 force)
	{
		this->bulletRigidBody->activate(true);

		this->bulletRigidBody->applyCentralForce(toBtVec(force));
	}
}