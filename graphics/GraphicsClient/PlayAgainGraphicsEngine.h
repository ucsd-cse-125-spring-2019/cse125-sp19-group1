#pragma once
#include "TwoDeeGraphicsEngine.h"

class PlayAgainGraphicsEngine : public TwoDeeGraphicsEngine
{
public:
	PlayAgainGraphicsEngine();
	~PlayAgainGraphicsEngine();

	virtual void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};

