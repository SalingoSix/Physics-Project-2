#ifndef _HG_CreateFactory_
#define _HG_CreateFactory_

#include "cBulletPhysicsFactory.h"

#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)

EXTERN_DLL_EXPORT nPhysics::iPhysicsFactory* CreateFactory();

#endif