#ifndef _HG_cBulletPhysicsWorld_
#define _HG_cBulletPhysicsWorld_

#include <iPhysicsWorld.h>
#include <vector>
#include <btBulletDynamicsCommon.h>
#include "cBulletRigidBody.h"

namespace nPhysics
{

	struct sRK4State
	{
		sRK4State(glm::vec3 a, glm::vec3 b, glm::vec3 c)
		{
			Position = a;
			Velocity = b;
			Acceleration = c;
		}
		sRK4State()
		{
			Position = glm::vec3{ 0.0f };
			Velocity = glm::vec3{ 0.0f };
			Acceleration = glm::vec3{ 0.0f };
		}
		glm::vec3 Position;
		glm::vec3 Velocity;
		glm::vec3 Acceleration;
	};

	class cBulletPhysicsWorld : public iPhysicsWorld
	{
	public:

		cBulletPhysicsWorld();

		virtual ~cBulletPhysicsWorld();

		virtual void TimeStep(float deltaTime);

		virtual void AddRigidBody(iRigidBody* rigidBody);
		virtual void RemoveRigidBody(iRigidBody* rigidBody);

	private:
		bool PenetrationTestSphereSphere(cBulletRigidBody* pA, cBulletRigidBody* pB);
		int intersect_moving_sphere_sphere(const glm::vec3& c0, float r0, const glm::vec3& v0, const glm::vec3& c1, float r1, const glm::vec3& v1, float& t);
		int intersect_moving_sphere_plane(const glm::vec3& c, float r, const glm::vec3& v, const glm::vec3& n, float d, float& t, glm::vec3& q);
		glm::vec3 ClosestPtPointTriangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c);
		sRK4State RK4Eval(const sRK4State &state, float dt, const sRK4State &derivative);
		void RK4(glm::vec3 &pos, glm::vec3 &vel, glm::vec3 &acc, float dt);
		btDiscreteDynamicsWorld* dynamicsWorld;
		std::vector<cBulletRigidBody*> mVecRigidBodies;
		btAlignedObjectArray<btCollisionShape*> collisionShapes;
	};
}

#endif