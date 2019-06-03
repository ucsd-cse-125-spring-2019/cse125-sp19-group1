#pragma once
#include "TwoDeeGraphicsEngine.h"

#include <cstdint>

class FBXObject;

class CutsceneGraphicsEngine : public TwoDeeGraphicsEngine
{
public:
	bool showSkippableMsg;

	CutsceneGraphicsEngine(const char *textureFilename, double duration = DBL_MAX);
	~CutsceneGraphicsEngine();

	virtual bool ShouldFadeout();
	virtual void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	virtual void MainLoopBegin();
	virtual void MainLoopEnd();
	virtual void MainLoopCallback(GLFWwindow * window);

	bool shouldSkipTheRest();

protected:
	double startTime;
	double duration;
	bool skipTheRest;

	static FBXObject *foregroundObj;
	static int foregroundObjRefCount;
};

