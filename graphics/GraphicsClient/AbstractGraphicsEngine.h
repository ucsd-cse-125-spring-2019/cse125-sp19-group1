#pragma once

#include "Core.h"

class AbstractGraphicsEngine
{
public:
	bool calledMainLoopBegin;
	bool fullyLoaded;  // true if the engine has finished loading and is ready to render

	virtual void StartLoading() = 0;  // may launch a thread and return immediately
	virtual void CleanUp() = 0;
	virtual void MainLoopBegin() = 0;
	virtual void MainLoopEnd() = 0;

	virtual void ResizeCallback(GLFWwindow* window, int newWidth, int newHeight) = 0;
	virtual void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) = 0;
	virtual void CursorPosCallback(GLFWwindow * window, double xpos, double ypos) = 0;
	virtual void MouseWheelCallback(GLFWwindow * window, double xoffset, double yoffset) = 0;
	virtual void MouseButtonCallback(GLFWwindow * window, int button, int action, int mods) = 0;
	virtual void MainLoopCallback(GLFWwindow * window) = 0;
};

