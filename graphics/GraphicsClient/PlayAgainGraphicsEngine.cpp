#include "PlayAgainGraphicsEngine.h"

#include "../../network/ClientGame.h"

#define TEXTURE_PATH "../2D Elements/"
#define BG_TEX_PATH (TEXTURE_PATH "endscreen_bkg.png")

extern ClientGame *sharedClient;


PlayAgainGraphicsEngine::PlayAgainGraphicsEngine()
{
	backgroundFilename = BG_TEX_PATH;
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