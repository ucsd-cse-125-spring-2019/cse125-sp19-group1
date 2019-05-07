#ifndef _TESTER_H_
#define _TESTER_H_

#include <process.h>
#include <chrono>
#include <thread>

#include "Core.h"
#include "DirLight.h"
#include "FogGenerator.h"
#include "FBXObject.h"
#include "Node.h"
#include "Transform.h"
#include "Geometry.h"
//#include "./server/ServerGame.h"
//#include "./server/ClientGame.h"
#include "../../network/ServerGame.h"
#include "../../network/ClientGame.h"

#define OBJ_VERT_SHADER_PATH "./obj_shader.vert"
#define OBJ_FRAG_SHADER_PATH "./obj_shader.frag"

#define ANIMATIONS_PATH  "../Animations/"
#define MODELS_PATH      "../Models/"
#define TEXTURES_PATH    "../Textures/"

#define RACCOON_DAE_PATH  (ANIMATIONS_PATH "raccoonWalk.dae")
#define RACCOON_MDL_PATH  (MODELS_PATH "raccoon.fbx")
#define RACCOON_TEX_PATH  (TEXTURES_PATH "raccoon.ppm")

#define CAT_MDL_PATH      (MODELS_PATH "cat.fbx")
#define CAT_TEX_PATH      (TEXTURES_PATH "cat.ppm")

#define DOG_MDL_PATH      (MODELS_PATH "doggo.fbx")
#define DOG_TEX_PATH      (TEXTURES_PATH "doggo.ppm")

#define CHEF_DAE_PATH     (ANIMATIONS_PATH "chefWalk.dae")
#define CHEF_MDL_PATH     (MODELS_PATH "chef.fbx")
#define CHEF_TEX_PATH     (TEXTURES_PATH "chef.ppm")

#define TILE_MDL_PATH     (MODELS_PATH "tile.fbx")
#define TILE_TEX_PATH     (TEXTURES_PATH "tile.ppm")


#define CHEF_FOG_DISTANCE 85.0f
#define RACCOON_FOG_DISTANCE 160

// function declarations
void ErrorCallback(int error, const char* description);
void SetupGlew();
void SetupOpenGLSettings();
void PrintVersions();
void Init();
void serverLoop(void * args);
void CleanUp();
void ResizeCallback(GLFWwindow* window, int newWidth, int newHeight);
GLFWwindow* CreateWindowFrame(int width, int height);
void SendPackets();
void MovePlayer();
void MoveCamera(glm::vec3 * newPlayerPos);
void IdleCallback();
void DisplayCallback(GLFWwindow* window);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void UpdateView();
glm::vec3 TrackballMapping(double x, double y, int width, int height);
void TrackballRotation(float rotationAngle, glm::vec3 rotationAxis);
void CursorPosCallback(GLFWwindow * window, double xpos, double ypos);
void MouseWheelCallback(GLFWwindow * window, double xoffset, double yoffset);
void MouseButtonCallback(GLFWwindow * window, int button, int action, int mods);
void SetupCallbacks();
int main(void);

#endif

