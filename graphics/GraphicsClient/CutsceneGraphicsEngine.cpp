#include "CutsceneGraphicsEngine.h"
#include "../../network/ClientGame.h"
#include "../../network/ClientNetwork.h"
#include "FBXObject.h"

#define TEXTURE_PATH "../2D Elements/"
#define MSG_TEX_PATH (TEXTURE_PATH "cutscene - skip button.png")

static const glm::mat4 identityMat(1.f);

extern ClientGame * sharedClient;

FBXObject *CutsceneGraphicsEngine::foregroundObj = nullptr;
int CutsceneGraphicsEngine::foregroundObjRefCount = 0;


CutsceneGraphicsEngine::CutsceneGraphicsEngine(const char *textureFilename, double duration) 
	: TwoDeeGraphicsEngine(), duration(duration), showSkippableMsg(true)
{
	backgroundFilename = textureFilename;
}


CutsceneGraphicsEngine::~CutsceneGraphicsEngine()
{
}


void CutsceneGraphicsEngine::MainLoopBegin()
{
	TwoDeeGraphicsEngine::MainLoopBegin();

	if (showSkippableMsg && foregroundObjRefCount++ <= 0) {
		foregroundObj = createObjectForTexture(MSG_TEX_PATH);
		foregroundObjRefCount = 1;
	}

	startTime = DBL_MAX;
	skipTheRest = false;
}

void CutsceneGraphicsEngine::MainLoopEnd()
{
	TwoDeeGraphicsEngine::MainLoopEnd();

	if (showSkippableMsg && --foregroundObjRefCount <= 0 && foregroundObj) {
		delete foregroundObj;
		foregroundObj = nullptr;
	}
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
		skipTheRest = true;
		// fall through to the SPACE case on purpose

	case GLFW_KEY_SPACE:
		if (sharedClient->getGameData()->getGameState() == GameState::WIN_CUTSCENE)
			sharedClient->sendPackets(GO_TO_CREDITS_EVENT);
		quit = true;
		break;

	default:
		break;
	}
}

void CutsceneGraphicsEngine::MainLoopCallback(GLFWwindow * window)
{
	TwoDeeGraphicsEngine::MainLoopCallback(window);

	double now = glfwGetTime();

	// Transition to the end credits if a player has hit space for the win cutscene
	sharedClient->update();
	if (sharedClient->getGameData()->getGameState() == GameState::END_CREDITS)
	{
		quit = true;
	}
//#define START_PERIOD 0.8
//	float alpha = sinf((now / START_PERIOD) * glm::pi<double>()) * 0.5f + 0.5f;

	float alpha = 0.75f;

	if (showSkippableMsg && foregroundObj) {
		glUniform1f(uAlpha, alpha * sqrt(screenAlpha));
		foregroundObj->Draw(passthroughShaderProgram, &identityMat, &orthoProj, identityMat);
	}
}

bool CutsceneGraphicsEngine::shouldSkipTheRest()
{
	return skipTheRest;
}
