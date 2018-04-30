#ifndef _HG_cBulletPlaneShape_
#define _HG_cBulletPlaneShape_

#include <glm/vec3.hpp>
#include <iShape.h>
#include <btBulletDynamicsCommon.h>

#include "BulletCalculations.h"

namespace nPhysics
{
	class cBulletPlaneShape : public iShape
	{
	public:
		cBulletPlaneShape(glm::vec3 cornerA, glm::vec3 cornerB);

		virtual ~cBulletPlaneShape();

		virtual int planeGetNormal();
		virtual bool planeGetBottomCorner(glm::vec3 &corner);
		virtual bool planeGetTopCorner(glm::vec3 &corner);
		virtual bool planeGetNormalVec(glm::vec3 &normal);
		virtual bool planeGetPlaneConst(float &planeConst);

		btCollisionShape* getBulletShape();

	private:
		glm::vec3 bottomCorner;
		glm::vec3 topCorner;
		glm::vec3 normalVec;
		float planeConst;
		int normalPlane; //1 for x, 2 for y, 3 for z
		btCollisionShape* bulletPlane;
	};
}

#endif