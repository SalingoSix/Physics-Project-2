#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "SceneHandler.h"
#include "cShaderManager.h"
#include "cMesh.h"
#include "cVAOMeshManager.h"
#include "cGameObject.h"
#include "cLightManager.h"

#include <iPhysicsFactory.h>
#include <iPhysicsWorld.h>
#include <iRigidBody.h>
#include <iShape.h>

typedef nPhysics::iPhysicsFactory*(*f_CreateFactory)();

nPhysics::iPhysicsFactory* g_bigFactory;
nPhysics::iPhysicsWorld* g_bigWorld;
int cameraFocus = 5;

//Camera variables
float angle = 0.0f;
float lookx = 0.0f, looky = 3.0f, lookz = -1.0f;
float camPosx = 0.0f, camPosy = 3.0f, camPosz = 10.0f;
glm::vec3 g_cameraXYZ = glm::vec3(0.0f, 3.0f, 15.0f);
glm::vec3 g_cameraTarget_XYZ = glm::vec3(0.0f, 3.0f, 0.0f);

//Global handlers for shader, VAOs, game objects and lights
cShaderManager* g_pShaderManager = new cShaderManager();
cVAOMeshManager* g_pVAOManager = new cVAOMeshManager();
std::vector <cMesh> g_vecMesh;
std::vector <cGameObject*> g_vecGameObject;
std::vector <glm::vec3> g_vecObjStart;
cLightManager* g_pLightManager = new cLightManager();

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	//Space turns the blue guy (or whatever models was loaded second) into a wireframe and back
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		cameraFocus++;
		if (cameraFocus >= ::g_vecGameObject.size())
		{
			cameraFocus = 5;
		}
	}
	
	//Keys 1 through 8 will turn on and off the 8 point lights on the scene
	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
		::g_pLightManager->vecLights[0].lightIsOn = !(::g_pLightManager->vecLights[0].lightIsOn);
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
		::g_pLightManager->vecLights[1].lightIsOn = !(::g_pLightManager->vecLights[1].lightIsOn);
	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
		::g_pLightManager->vecLights[2].lightIsOn = !(::g_pLightManager->vecLights[2].lightIsOn);
	if (key == GLFW_KEY_4 && action == GLFW_PRESS)
		::g_pLightManager->vecLights[3].lightIsOn = !(::g_pLightManager->vecLights[3].lightIsOn);
	if (key == GLFW_KEY_5 && action == GLFW_PRESS)
		::g_pLightManager->vecLights[4].lightIsOn = !(::g_pLightManager->vecLights[4].lightIsOn);
	if (key == GLFW_KEY_6 && action == GLFW_PRESS)
		::g_pLightManager->vecLights[5].lightIsOn = !(::g_pLightManager->vecLights[5].lightIsOn);
	if (key == GLFW_KEY_7 && action == GLFW_PRESS)
		::g_pLightManager->vecLights[6].lightIsOn = !(::g_pLightManager->vecLights[6].lightIsOn);
	if (key == GLFW_KEY_8 && action == GLFW_PRESS)
		::g_pLightManager->vecLights[7].lightIsOn = !(::g_pLightManager->vecLights[7].lightIsOn);

	const float CAMERASPEED = 0.3f;
	switch (key)
	{
	//MOVEMENT USING TANK CONTROLS WASD
	case GLFW_KEY_A:		// Look Left
		::g_vecGameObject[cameraFocus]->myBody->applyForce(glm::vec3(-5.0f, 0.0f, 0.0f));
		break;
	case GLFW_KEY_D:		// Look Right
		::g_vecGameObject[cameraFocus]->myBody->applyForce(glm::vec3(5.0f, 0.0f, 0.0f));
		break;
	case GLFW_KEY_W:		// Move Forward (relative to which way you're facing)
		::g_vecGameObject[cameraFocus]->myBody->applyForce(glm::vec3(0.0f, 0.0f, -5.0f));
		break;
	case GLFW_KEY_S:		// Move Backward
		::g_vecGameObject[cameraFocus]->myBody->applyForce(glm::vec3(0.0f, 0.0f, 5.0f));
		break;
	case GLFW_KEY_DOWN:		// Look Down (along y axis)
		//if (g_cameraTarget_XYZ.y > 0.5)	//Up and down looking range is limited
		camPosy -= CAMERASPEED;
		if (camPosy < 0.0f)
			camPosy = 0.0f;
		break;
	case GLFW_KEY_UP:		// Look Up (along y axis)
		//if (g_cameraTarget_XYZ.y < 1.5)
		camPosy += CAMERASPEED;
		if (camPosy > 35.0f)
			camPosy = 35.0f;
		break;
	//case GLFW_KEY_I:
	//	g_vecGameObject[4]->vel.z -= 0.1;
	//	break;
	//case GLFW_KEY_K:
	//	g_vecGameObject[4]->vel.z += 0.1;
	//	break;
	//case GLFW_KEY_J:
	//	g_vecGameObject[4]->vel.x -= 0.1;
	//	break;
	//case GLFW_KEY_L:
	//	g_vecGameObject[4]->vel.x += 0.1;
	//	break;
	}
}

int main()
{

	std::string libraryFile = "BulletPhysicsLibrary.dll";
	//std::string libraryFile = "PhysicsLibrary.dll";
	std::string createFactoryName = "CreateFactory";

	HINSTANCE hGetProckDll = LoadLibraryA(libraryFile.c_str());
	if (!hGetProckDll)
	{
		std::cout << "oh no!" << std::endl;
		system("pause");
		return 1;
	}

	f_CreateFactory CreateFactory = 0;

	CreateFactory = (f_CreateFactory)GetProcAddress(hGetProckDll, createFactoryName.c_str());
	if (!CreateFactory)
	{
		std::cout << "where's the CreateFactory???" << std::endl;
		system("pause");
		return 1;
	}

	::g_bigFactory = CreateFactory();

	::g_bigWorld = ::g_bigFactory->CreateWorld();

	GLFWwindow* window;

	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	window = glfwCreateWindow(1080, 720,
		"Balls Party",
		NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);

	cShaderManager::cShader vertShader;
	cShaderManager::cShader fragShader;

	vertShader.fileName = "simpleVert.glsl";
	fragShader.fileName = "simpleFrag.glsl";

	::g_pShaderManager->setBasePath("assets//shaders//");

	if (!::g_pShaderManager->createProgramFromFile(
		"simpleShader", vertShader, fragShader))
	{
		std::cout << "Failed to create shader program. Shutting down." << std::endl;
		std::cout << ::g_pShaderManager->getLastError() << std::endl;
		return -1;
	}
	std::cout << "The shaders comipled and linked OK" << std::endl;
	GLint shaderID = ::g_pShaderManager->getIDFromFriendlyName("simpleShader");

	std::string* plyDirects;
	std::string* plyNames;
	int numPlys;

	if (!readPlysToLoad(&plyDirects, &plyNames, &numPlys))
	{
		std::cout << "Couldn't find files to read. Giving up hard.";
		return -1;
	}

	for (int i = 0; i < numPlys; i++)
	{	//Load each file into a VAO object
		cMesh newMesh;
		newMesh.name = plyNames[i];
		if (!LoadPlyFileIntoMesh(plyDirects[i], newMesh))
		{
			std::cout << "Didn't load model" << std::endl;
		}
		g_vecMesh.push_back(newMesh);
		if (!::g_pVAOManager->loadMeshIntoVAO(newMesh, shaderID))
		{
			std::cout << "Could not load mesh into VAO" << std::endl;
		}
	}

	int numEntities;
	if (!readEntityDetails(&g_vecGameObject, &numEntities, &g_vecObjStart))
	{
		std::cout << "Unable to find game objects for placement." << std::endl;
	}


	std::cout << glGetString(GL_VENDOR) << " "
		<< glGetString(GL_RENDERER) << ", "
		<< glGetString(GL_VERSION) << std::endl;
	std::cout << "Shader language version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	GLint currentProgID = ::g_pShaderManager->getIDFromFriendlyName("simpleShader");

	GLint uniLoc_mModel = glGetUniformLocation(currentProgID, "mModel");
	GLint uniLoc_mView = glGetUniformLocation(currentProgID, "mView");
	GLint uniLoc_mProjection = glGetUniformLocation(currentProgID, "mProjection");

	GLint uniLoc_materialDiffuse = glGetUniformLocation(currentProgID, "materialDiffuse");

	::g_pLightManager->createLights();	// There are 10 lights in the shader
	::g_pLightManager->LoadShaderUniformLocations(currentProgID);

	glEnable(GL_DEPTH);

	double lastTimeStep = glfwGetTime();

	while (!glfwWindowShouldClose(window))
	{
		float ratio;
		int width, height;
		glm::mat4x4 m, p;

		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;
		glViewport(0, 0, width, height);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		::g_pLightManager->CopyInfoToShader();

		unsigned int sizeOfVector = ::g_vecGameObject.size();	//*****//
		for (int index = 0; index != sizeOfVector; index++)
		{
			// Is there a game object? 
			if (::g_vecGameObject[index] == 0)	//if ( ::g_GameObjects[index] == 0 )
			{	// Nothing to draw
				continue;		// Skip all for loop code and go to next
			}

			// Was near the draw call, but we need the mesh name
			std::string meshToDraw = ::g_vecGameObject[index]->meshName;		//::g_GameObjects[index]->meshName;

			sVAOInfo VAODrawInfo;
			if (::g_pVAOManager->lookupVAOFromName(meshToDraw, VAODrawInfo) == false)
			{	// Didn't find mesh
				continue;
			}

			m = glm::mat4x4(1.0f);	

			glm::vec3 myPosition;
			::g_vecGameObject[index]->myBody->getPosition(myPosition);

			glm::mat4 trans = glm::mat4x4(1.0f);
			trans = glm::translate(trans,
				myPosition);
			m = m * trans;

			glm::vec3 myRotation;
			::g_vecGameObject[index]->myBody->getRotataion(myRotation);

			glm::mat4 matPostRotZ = glm::mat4x4(1.0f);
			matPostRotZ = glm::rotate(matPostRotZ, myRotation.z,
				glm::vec3(0.0f, 0.0f, 1.0f));
			m = m * matPostRotZ;

			glm::mat4 matPostRotY = glm::mat4x4(1.0f);
			matPostRotY = glm::rotate(matPostRotY, myRotation.y,
				glm::vec3(0.0f, 1.0f, 0.0f));
			m = m * matPostRotY;

			glm::mat4 matPostRotX = glm::mat4x4(1.0f);
			matPostRotX = glm::rotate(matPostRotX, myRotation.x,
				glm::vec3(1.0f, 0.0f, 0.0f));
			m = m * matPostRotX;

			if (::g_vecGameObject[index]->myBody->getShape()->getShapeType() == nPhysics::SHAPE_SPHERE)
			{
				float finalScale;
				::g_vecGameObject[index]->myBody->getShape()->sphereGetRadius(finalScale);
				glm::mat4 matScale = glm::mat4x4(1.0f);
				matScale = glm::scale(matScale,
					glm::vec3(finalScale,
						finalScale,
						finalScale));
				m = m * matScale;
			}

			p = glm::perspective(0.6f,			// FOV
				ratio,		// Aspect ratio
				0.1f,			// Near (as big as possible)
				1000.0f);	// Far (as small as possible)

							// View or "camera" matrix
			glm::mat4 v = glm::mat4(1.0f);	// identity

			g_cameraXYZ.x = camPosx;
			g_cameraXYZ.y = camPosy;
			g_cameraXYZ.z = camPosz;

			glm::vec3 targetMan;
			::g_vecGameObject[cameraFocus]->myBody->getPosition(targetMan);
			g_cameraTarget_XYZ.x = targetMan.x;
			g_cameraTarget_XYZ.y = targetMan.y;
			g_cameraTarget_XYZ.z = targetMan.z;
			//g_cameraTarget_XYZ.x = camPosx + lookx;
			//g_cameraTarget_XYZ.y = camPosy;
			//g_cameraTarget_XYZ.z = camPosz + lookz;

			v = glm::lookAt(g_cameraXYZ,						// "eye" or "camera" position
				g_cameraTarget_XYZ,		// "At" or "target" 
				glm::vec3(0.0f, 1.0f, 0.0f));	// "up" vector

			if (index != cameraFocus && index > 4)
			{
				glUniform4f(uniLoc_materialDiffuse,
					0.5f,
					0.5f,
					0.5f,
					::g_vecGameObject[index]->diffuseColour.a);
			}
			else
			{
				glUniform4f(uniLoc_materialDiffuse,
					::g_vecGameObject[index]->diffuseColour.r,
					::g_vecGameObject[index]->diffuseColour.g,
					::g_vecGameObject[index]->diffuseColour.b,
					::g_vecGameObject[index]->diffuseColour.a);
			}



			//        glUseProgram(program);
			::g_pShaderManager->useShaderProgram("simpleShader");

			glUniformMatrix4fv(uniLoc_mModel, 1, GL_FALSE,
				(const GLfloat*)glm::value_ptr(m));

			glUniformMatrix4fv(uniLoc_mView, 1, GL_FALSE,
				(const GLfloat*)glm::value_ptr(v));

			glUniformMatrix4fv(uniLoc_mProjection, 1, GL_FALSE,
				(const GLfloat*)glm::value_ptr(p));

			if(::g_vecGameObject[index]->wireFrame)
				glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			else
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			glBindVertexArray(VAODrawInfo.VAO_ID);

			glDrawElements(GL_TRIANGLES,
				VAODrawInfo.numberOfIndices,
				GL_UNSIGNED_INT,
				0);

			glBindVertexArray(0);

		}//for ( int index = 0...

		double curTime = glfwGetTime();
		double deltaTime = curTime - lastTimeStep;

		::g_bigWorld->TimeStep(deltaTime);

		lastTimeStep = curTime;

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}