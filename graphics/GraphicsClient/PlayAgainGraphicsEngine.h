#pragma once
#include "TwoDeeGraphicsEngine.h"

class FBXObject;

class PlayAgainGraphicsEngine : public TwoDeeGraphicsEngine
{
public:
	PlayAgainGraphicsEngine();
	~PlayAgainGraphicsEngine();

	virtual void MainLoopBegin();
	virtual void MainLoopEnd();
	virtual void MainLoopCallback(GLFWwindow * window);
	virtual void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

protected:
	FBXObject * foregroundObj;
};

