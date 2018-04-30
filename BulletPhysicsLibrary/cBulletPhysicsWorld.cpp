#include "cBulletPhysicsWorld.h"

namespace nPhysics
{
	cBulletPhysicsWorld::cBulletPhysicsWorld()
	{
		btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();

		///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
		btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

		///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
		btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();

		///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
		btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

		dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

		dynamicsWorld->setGravity(btVector3(0, -10, 0));

		///-----initialization_end-----
	}
	cBulletPhysicsWorld::~cBulletPhysicsWorld()
	{
		//remove the rigidbodies from the dynamics world and delete them
		for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
		{
			btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
			btRigidBody* body = btRigidBody::upcast(obj);
			if (body && body->getMotionState())
			{
				delete body->getMotionState();
			}
			dynamicsWorld->removeCollisionObject(obj);
			delete obj;
		}

		//delete collision shapes
		for (int j = 0; j < collisionShapes.size(); j++)
		{
			btCollisionShape* shape = collisionShapes[j];
			collisionShapes[j] = 0;
			delete shape;
		}

		//delete dynamics world
		delete dynamicsWorld;
	}

	void cBulletPhysicsWorld::TimeStep(float deltaTime)
	{

		dynamicsWorld->stepSimulation(1.f / 60.f, 10);

	}

	void cBulletPhysicsWorld::AddRigidBody(iRigidBody* rigidBody)
	{
		cBulletRigidBody* castBody = (cBulletRigidBody*)rigidBody;
		if (castBody == NULL)
			return;

		for (int index = 0; index < dynamicsWorld->getNumCollisionObjects(); index++)
		{
			btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[index];
			btRigidBody* body = btRigidBody::upcast(obj);
			if (body == castBody->bulletRigidBody)
				return;
		}

		dynamicsWorld->addRigidBody(castBody->bulletRigidBody);
		return;
	}

	void cBulletPhysicsWorld::RemoveRigidBody(iRigidBody* rigidBody)
	{
		cBulletRigidBody* castBody = (cBulletRigidBody*)rigidBody;
		if (castBody == NULL)
			return;

		for (int index = 0; index < dynamicsWorld->getNumCollisionObjects(); index++)
		{
			btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[index];
			btRigidBody* body = btRigidBody::upcast(obj);
			if (body == castBody->bulletRigidBody)
			{
				//dynamicsWorld->removeRigidBody(castBody->bulletRigidBody);
				dynamicsWorld->removeCollisionObject(obj);
				return;
			}
		}
		return;
	}

	bool cBulletPhysicsWorld::PenetrationTestSphereSphere(cBulletRigidBody* pA, cBulletRigidBody* pB)
	{
		float radiusA, radiusB;
		pA->mShape->sphereGetRadius(radiusA);
		pB->mShape->sphereGetRadius(radiusB);
		float totalRadii = radiusA + radiusB;

		// The Pythagorean distance 
		float distance = glm::distance(pA->mPosition, pB->mPosition);

		if (distance <= totalRadii)
		{
			return true;
		}

		return false;
	}

	int cBulletPhysicsWorld::intersect_moving_sphere_sphere(
		const glm::vec3& c0, float r0, const glm::vec3& v0,
		const glm::vec3& c1, float r1, const glm::vec3& v1, float& t)
	{
		glm::vec3 s = c1 - c0;  // sphere center separation
		glm::vec3 v = v1 - v0;  // relative motion of sphere 1 w.r.t. stationary s0
		float r = r1 + r0; // sum of sphere radii
		float c = dot(s, s) - r * r;
		if (c < 0.f)
		{
			// spheres initially overlapping, exit early
			t = 0.f;
			return -1;
		}
		float a = dot(v, v);
		if (a < FLT_EPSILON) return 0; // spheres not moving relative to eachother
		float b = dot(v, s);
		if (b >= 0.f) return 0; // spheres not moving towards eachother
		float d = b * b - a * c;
		if (d < 0.f) return 0;  // no real root, so spheres do not intersect

		t = (-b - sqrt(d)) / a;
		return t < 1.f ? 1 : 0;
	}

	// c : sphere center
	// r : sphere radius
	// v : sphere movement
	// n : plane normal
	// d : plane dot product
	// t : output : [0,1] collision
	// q : output : collision point, where the sphere first contacts the plane
	// returns true/false whether the sphere collides with the plane in [c, c + v]
	int cBulletPhysicsWorld::intersect_moving_sphere_plane(const glm::vec3& c, float r, const glm::vec3& v, const glm::vec3& n, float d, float& t, glm::vec3& q)
	{
		float dist = dot(n, c) - d;
		if (abs(dist) <= r)
		{
			// sphere is already overlapping the plane.
			// set time of intersection to 0 and q to sphere center
			t = 0.f;
			q = c;
			return -1;
		}
		else
		{
			float denom = dot(n, v);
			if (denom * dist >= 0.f)
			{
				// no intersection as sphere moving parallel to or away from plane
				return 0;
			}
			else
			{
				// sphere is moving towards the plane
				// use +r in computations if sphere in front of plane, else -r
				float rad = dist > 0.f ? r : -r;
				t = (r - dist) / denom;
				q = c + (v * t) - (n * rad);
				return t <= 1.f ? 1 : 0;
			}
		}
	}

	glm::vec3 cBulletPhysicsWorld::ClosestPtPointTriangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c)
	{
		// Check if P in vertex region outside A
		glm::vec3 ab = b - a;
		glm::vec3 ac = c - a;
		glm::vec3 ap = p - a;
		float d1 = glm::dot(ab, ap);
		float d2 = glm::dot(ac, ap);
		if (d1 <= 0.0f && d2 <= 0.0f) return a; // barycentric coordinates (1,0,0)

												// Check if P in vertex region outside B
		glm::vec3 bp = p - b;
		float d3 = glm::dot(ab, bp);
		float d4 = glm::dot(ac, bp);
		if (d3 >= 0.0f && d4 <= d3) return b; // barycentric coordinates (0,1,0)

											  // Check if P in edge region of AB, if so return projection of P onto AB
		float vc = d1 * d4 - d3 * d2;
		if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
			float v = d1 / (d1 - d3);
			return a + v * ab; // barycentric coordinates (1-v,v,0)
		}

		// Check if P in vertex region outside C
		glm::vec3 cp = p - c;
		float d5 = glm::dot(ab, cp);
		float d6 = glm::dot(ac, cp);
		if (d6 >= 0.0f && d5 <= d6) return c; // barycentric coordinates (0,0,1)

											  // Check if P in edge region of AC, if so return projection of P onto AC
		float vb = d5 * d2 - d1 * d6;
		if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
			float w = d2 / (d2 - d6);
			return a + w * ac; // barycentric coordinates (1-w,0,w)
		}

		// Check if P in edge region of BC, if so return projection of P onto BC
		float va = d3 * d6 - d5 * d4;
		if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
			float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
			return b + w * (c - b); // barycentric coordinates (0,1-w,w)
		}

		// P inside face region. Compute Q through its barycentric coordinates (u,v,w)
		float denom = 1.0f / (va + vb + vc);
		float v = vb * denom;
		float w = vc * denom;
		return a + ab * v + ac * w; // = u*a + v*b + w*c, u = va * denom = 1.0f - v - w
	}

	sRK4State cBulletPhysicsWorld::RK4Eval(const sRK4State &state, float dt, const sRK4State &derivative)
	{
		sRK4State s;
		s.Position = state.Position + derivative.Position * dt;
		s.Velocity = state.Velocity + derivative.Velocity * dt;

		sRK4State dS;
		dS.Position = s.Velocity;
		dS.Velocity = state.Acceleration;
		return dS;
	}

	void cBulletPhysicsWorld::RK4(glm::vec3 &pos, glm::vec3 &vel, glm::vec3 &acc, float dt)
	{
		float halfDt = dt * 0.5f;
		sRK4State state(pos, vel, acc);
		sRK4State a = RK4Eval(state, 0.0f, sRK4State());
		sRK4State b = RK4Eval(state, halfDt, a);
		sRK4State c = RK4Eval(state, halfDt, b);
		sRK4State d = RK4Eval(state, dt, c);

		glm::vec3 rkVel = (a.Position + ((b.Position + c.Position) * 2.0f) + d.Position);// *(1.0f / 6.0f);
		glm::vec3 rkAcc = (a.Velocity + ((b.Velocity + c.Velocity) * 2.0f) + d.Velocity);// *(1.0f / 6.0f);

		pos += rkVel * dt;
		vel += rkAcc * dt;
	}
}