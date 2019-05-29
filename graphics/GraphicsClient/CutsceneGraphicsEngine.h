#pragma once
#include "TwoDeeGraphicsEngine.h"

class CutsceneGraphicsEngine : public TwoDeeGraphicsEngine
{
public:
	CutsceneGraphicsEngine(const char *textureFilename, double duration = 7.0);
	~CutsceneGraphicsEngine();

	virtual bool ShouldFadeout();
	virtual void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	virtual void MainLoopBegin();

protected:
	double startTime;
	double duration;
};

