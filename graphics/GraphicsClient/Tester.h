#ifndef _TESTER_H_
#define _TESTER_H_

#include "Core.h"
#include "DirLight.h"
#include "FBXObject.h"
#include <process.h>
#include "./server/ServerGame.h"
#include "./server/ClientGame.h"

#define OBJ_VERT_SHADER_PATH "./obj_shader.vert"
#define OBJ_FRAG_SHADER_PATH "./obj_shader.frag"
#define DOOR_PATH "../Models/chair.fbx"
#define TABLE_PATH "../Models/Table.obj"

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

