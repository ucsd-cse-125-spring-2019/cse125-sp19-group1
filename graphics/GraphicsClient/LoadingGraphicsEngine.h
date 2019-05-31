#pragma once
#include "TwoDeeGraphicsEngine.h"

class FBXObject;

class LoadingGraphicsEngine : public TwoDeeGraphicsEngine
{
public:
	bool temporarilySuppressAnimation;

	LoadingGraphicsEngine();
	~LoadingGraphicsEngine();

	virtual void StartLoading();  // may launch a thread and return immediately
	virtual void CleanUp();
	virtual void MainLoopBegin();
	virtual void MainLoopEnd();

	virtual void ResizeCallback(GLFWwindow* window, int newWidth, int newHeight);
	virtual void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	virtual void CursorPosCallback(GLFWwindow * window, double xpos, double ypos);
	virtual void MouseWheelCallback(GLFWwindow * window, double xoffset, double yoffset);
	virtual void MouseButtonCallback(GLFWwindow * window, int button, int action, int mods);
	virtual void MainLoopCallback(GLFWwindow * window);

protected:
	void drawDot(glm::vec3 position, float alpha);
	void drawPaw(glm::vec3 position, float alpha, float angle);

	FBXObject *dotObj = nullptr;
	FBXObject *pawObj = nullptr;
};

