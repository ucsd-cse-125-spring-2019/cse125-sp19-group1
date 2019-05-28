#include "TwoDeeGraphicsEngine.h"

#include "core.h"
#include "shader.h"
#include "FBXObject.h"

#define CANVAS_PATH      "../Canvas/"
#define CANVAS_MDL_PATH  (CANVAS_PATH "canvas2.fbx")

static const glm::mat4 identityMat(1.f);


FBXObject * createObjectForTexture(const char *texturePath)
{
	auto obj = new FBXObject(CANVAS_MDL_PATH, texturePath, false, GL_LINEAR);
	obj->SetDepthTest(false);

	return obj;
}


TwoDeeGraphicsEngine::TwoDeeGraphicsEngine()
{
	fullyLoaded = false;
	calledMainLoopBegin = false;
	screenAlpha = 1.f;
}


TwoDeeGraphicsEngine::~TwoDeeGraphicsEngine()
{
}


void TwoDeeGraphicsEngine::StartLoading()  // may launch a thread and return immediately
{
	fullyLoaded = true;

}

void TwoDeeGraphicsEngine::CleanUp()
{
	if (backgroundObj) {
		delete backgroundObj;
		backgroundObj = nullptr;
	}

	if (passthroughShaderProgram) {
		glDeleteShader(passthroughShaderProgram);
		passthroughShaderProgram = 0;
	}
}

void TwoDeeGraphicsEngine::MainLoopBegin()
{
	calledMainLoopBegin = true;

	passthroughShaderProgram = LoadShaders("./passthrough.vert", "./passthrough.frag");

	backgroundObj = createObjectForTexture(backgroundFilename);

	// Set clear color
	glClearColor(0.f, 0.f, 0.f, 1.0f);

	orthoProj = glm::ortho(-1.f, 1.f, -1.f, 1.f);
}

void TwoDeeGraphicsEngine::MainLoopEnd()
{
	CleanUp();

	calledMainLoopBegin = false;
}

void TwoDeeGraphicsEngine::ResizeCallback(GLFWwindow* window, int newWidth, int newHeight)
{
	// Set the viewport size. This is the only matrix that OpenGL maintains for us in modern OpenGL!
	glViewport(0, 0, windowWidth, windowHeight);
}

void TwoDeeGraphicsEngine::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

}

void TwoDeeGraphicsEngine::CursorPosCallback(GLFWwindow * window, double xpos, double ypos)
{

}

void TwoDeeGraphicsEngine::MouseWheelCallback(GLFWwindow * window, double xoffset, double yoffset)
{

}

void TwoDeeGraphicsEngine::MouseButtonCallback(GLFWwindow * window, int button, int action, int mods)
{

}

void TwoDeeGraphicsEngine::MainLoopCallback(GLFWwindow * window)
{
	glViewport(0, 0, windowWidth, windowHeight);
	// Clear the color and depth buffers

	if (screenAlpha >= 1.f) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glm::mat4 bgMat = glm::mat4(1.f);

	glUseProgram(passthroughShaderProgram);

	glUniform1f(uAlpha, screenAlpha);
	backgroundObj->Draw(passthroughShaderProgram, &identityMat, &orthoProj, bgMat);

}
