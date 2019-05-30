#include "PlayAgainGraphicsEngine.h"

#include "FBXObject.h"
#include "../../network/ClientGame.h"

#define TEXTURE_PATH "../2D Elements/"
#define BG_TEX_PATH (TEXTURE_PATH "endscreen_bkg.png")
#define MSG_TEX_PATH (TEXTURE_PATH "endscreen_playagain.png")

extern ClientGame *sharedClient;

static const glm::mat4 identityMat(1.f);


PlayAgainGraphicsEngine::PlayAgainGraphicsEngine()
{
	backgroundFilename = BG_TEX_PATH;
	foregroundObj = nullptr;
}


PlayAgainGraphicsEngine::~PlayAgainGraphicsEngine()
{
}


void PlayAgainGraphicsEngine::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// We are only interested in key presses
	if (action != GLFW_PRESS) {
		return;
	}

	switch (key) {
	case GLFW_KEY_ENTER:
	case GLFW_KEY_SPACE:
		sharedClient->sendPackets(RESET_EVENT);
		quit = true;
		break;

	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, GL_TRUE);
		break;

	default:
		break;
	}
}


void PlayAgainGraphicsEngine::MainLoopBegin()
{
	TwoDeeGraphicsEngine::MainLoopBegin();

	if (!foregroundObj) {
		foregroundObj = createObjectForTexture(MSG_TEX_PATH);
	}
}


void PlayAgainGraphicsEngine::MainLoopEnd()
{
	TwoDeeGraphicsEngine::MainLoopEnd();

	if (foregroundObj) {
		delete foregroundObj;
		foregroundObj = nullptr;
	}
}


void PlayAgainGraphicsEngine::MainLoopCallback(GLFWwindow * window)
{
	TwoDeeGraphicsEngine::MainLoopCallback(window);

	double now = glfwGetTime();

#define START_PERIOD 0.8
	float alpha = sinf((now / START_PERIOD) * glm::pi<double>()) * 0.5f + 0.5f;

	if (foregroundObj) {
		glUniform1f(uAlpha, alpha * screenAlpha);
		foregroundObj->Draw(passthroughShaderProgram, &identityMat, &orthoProj, identityMat);
	}
}