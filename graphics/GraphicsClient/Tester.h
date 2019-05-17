#ifndef _TESTER_H_
#define _TESTER_H_

#include <process.h>
#include <chrono>
#include <thread>

#include "Core.h"
#include "DirLight.h"
#include "FogGenerator.h"
#include "FBXObject.h"
#include "UIObject.h"
#include "Node.h"
#include "Transform.h"
#include "Geometry.h"
#include "SoundSystem.h"
//#include "./server/ServerGame.h"
//#include "./server/ClientGame.h"
#include "../../network/ServerGame.h"
#include "../../network/ClientGame.h"

#define OBJ_VERT_SHADER_PATH "./obj_shader.vert"
#define OBJ_FRAG_SHADER_PATH "./obj_shader.frag"

#define UI_VERT_SHADER_PATH "./ui_shader.vert"
#define UI_FRAG_SHADER_PATH "./ui_shader.frag"

#define ANIMATIONS_PATH  "../Animations/"
#define MODELS_PATH      "../Models/"
#define TEXTURES_PATH    "../Textures/"

#define RACCOON_DAE_PATH  (ANIMATIONS_PATH "raccoonWalk.dae")
#define RACCOON_MDL_PATH  (MODELS_PATH "raccoon.fbx")
#define RACCOON_TEX_PATH  (TEXTURES_PATH "raccoon.ppm")

#define CAT_DAE_PATH      (ANIMATIONS_PATH "catWalk.dae")
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
#define WALL_MDL_PATH     (MODELS_PATH "wall.fbx")
#define WALL_TEX_PATH     (TEXTURES_PATH "wall.ppm")

#define CANVAS_MDL_PATH     (MODELS_PATH "canvas.fbx")
#define CANVAS_TEX_PATH		(TEXTURES_PATH "canvas.ppm");

// Paths for sounds
#define SOUNDS_PATH			"../../sounds/"
#define SOUNDS_DOOR			(SOUNDS_PATH "frontexit_door_short.mp3")
#define SOUNDS_DOOR_UNLOCK	(SOUNDS_PATH "frontexit_unlock_short.mp3")
#define SOUNDS_FOUND_ITEM	(SOUNDS_PATH "found_item_short.mp3")
#define SOUNDS_NET			(SOUNDS_PATH "pop.mp3")	// FIXME placeholder for swoosh
#define SOUNDS_RACCOON_UP	(SOUNDS_PATH "RaccoonDownUp.mp3")
#define SOUNDS_RACCOON_DOWN	(SOUNDS_PATH "RaccoonUpDown.mp3")
#define SOUNDS_SEARCH_ITEM	(SOUNDS_PATH "search_item.mp3")
#define SOUNDS_TOILET		(SOUNDS_PATH "bathroom_toilet.mp3")
#define SOUNDS_VENT_SCREW	(SOUNDS_PATH "ventexit_screw.mp3")
#define SOUNDS_WINDOW		(SOUNDS_PATH "bathroom_window.mp3")
#define SOUNDS_YAY			(SOUNDS_PATH "Yay.mp3")

// function declarations
glm::vec3 directionBitmaskToVector(int bitmask);
void ErrorCallback(int error, const char* description);
void SetupGlew();
void SetupOpenGLSettings();
void PrintVersions();
void Init();
void serverLoop(void * args);
void deallocFloor();
void CleanUp();
void ResizeCallback(GLFWwindow* window, int newWidth, int newHeight);
GLFWwindow* CreateWindowFrame(int width, int height);
void SendPackets();
void MovePlayers();
void MoveCamera(const glm::vec3 &newPlayerPos);
void MoveCamera(const glm::vec3 &newPlayerPos, const glm::vec3 &oldPlayerPos);
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

