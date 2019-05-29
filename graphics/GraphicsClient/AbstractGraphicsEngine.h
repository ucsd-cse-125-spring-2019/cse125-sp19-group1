#pragma once

#include "Core.h"

extern int windowWidth, windowHeight;

class AbstractGraphicsEngine
{
public:
	bool calledMainLoopBegin = false;
	bool fullyLoaded = false;  // true if the engine has finished loading and is ready to render
	float screenAlpha = 1.0f;

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

	virtual bool ShouldFadeout() = 0;
};

