#ifndef _TESTER_H_
#define _TESTER_H_

#include <process.h>
#include <chrono>
#include <thread>

#include "Core.h"

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
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void CursorPosCallback(GLFWwindow * window, double xpos, double ypos);
void MouseWheelCallback(GLFWwindow * window, double xoffset, double yoffset);
void MouseButtonCallback(GLFWwindow * window, int button, int action, int mods);
void SetupCallbacks();
int main(void);

#endif

