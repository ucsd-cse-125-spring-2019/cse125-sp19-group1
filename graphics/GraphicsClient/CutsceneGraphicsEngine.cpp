#include "CutsceneGraphicsEngine.h"



CutsceneGraphicsEngine::CutsceneGraphicsEngine(const char *textureFilename, double duration) 
	: TwoDeeGraphicsEngine(), duration(duration)
{
	backgroundFilename = textureFilename;
}


CutsceneGraphicsEngine::~CutsceneGraphicsEngine()
{
}


void CutsceneGraphicsEngine::MainLoopBegin()
{
	TwoDeeGraphicsEngine::MainLoopBegin();

	startTime = DBL_MAX;
}

bool CutsceneGraphicsEngine::ShouldFadeout()
{
	if (startTime == DBL_MAX) {
		startTime = glfwGetTime();
	}

	return quit || (glfwGetTime() - startTime) >= duration;
}


void CutsceneGraphicsEngine::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// We are only interested in key presses
	if (action != GLFW_PRESS) {
		return;
	}

	switch (key) {
	case GLFW_KEY_ENTER:
	case GLFW_KEY_SPACE:
		quit = true;
		break;

	default:
		break;
	}
}