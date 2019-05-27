#include "LoadingGraphicsEngine.h"

#include "core.h"
#include "shader.h"
#include "FBXObject.h"

#include <cassert>

#define TEXTURE_PATH "../2D Elements/"
#define BG_TEX_PATH (TEXTURE_PATH "loadingscreen_bkg.png")
#define DOT_TEX_PATH (TEXTURE_PATH "loadingscreen_dot.png")
#define PAW_TEX_PATH (TEXTURE_PATH "loadingscreen_paw.png")

#define CANVAS_PATH      "../Canvas/"
#define CANVAS_MDL_PATH  (CANVAS_PATH "canvas2.fbx")

#define PAW_ANGLE_OFFSET (-0.18f * glm::pi<float>())
#define PAW_MAX_ALPHA (0.81f)

static const glm::mat4 identifyMat(1.f);
static GLuint passthroughShaderProgram = 0;
GLuint uAlpha = 0;
FBXObject *backgroundObj = nullptr;
FBXObject *dotObj = nullptr;
FBXObject *pawObj = nullptr;


static FBXObject * createObjectForTexture(const char *texturePath)
{
	auto obj = new FBXObject(CANVAS_MDL_PATH, texturePath, false, GL_LINEAR);
	obj->SetDepthTest(false);

	return obj;
}

LoadingGraphicsEngine::LoadingGraphicsEngine()
{
	fullyLoaded = false;
	calledMainLoopBegin = false;
	loadingAlpha = 1.f;
}

LoadingGraphicsEngine::~LoadingGraphicsEngine()
{

}

void LoadingGraphicsEngine::StartLoading()  // may launch a thread and return immediately
{
	fullyLoaded = true;
}

void LoadingGraphicsEngine::CleanUp()
{
	if (backgroundObj) {
		delete backgroundObj;
		backgroundObj = nullptr;
	}

	if (dotObj) {
		delete dotObj;
		dotObj = nullptr;
	}

	if (pawObj) {
		delete pawObj;
		pawObj = nullptr;
	}

	if (passthroughShaderProgram) {
		glDeleteShader(passthroughShaderProgram);
		passthroughShaderProgram = 0;
	}
}

void LoadingGraphicsEngine::MainLoopBegin()
{
	calledMainLoopBegin = true;

	passthroughShaderProgram = LoadShaders("./passthrough.vert", "./passthrough.frag");

	backgroundObj = createObjectForTexture(BG_TEX_PATH);
	dotObj = createObjectForTexture(DOT_TEX_PATH);
	pawObj = createObjectForTexture(PAW_TEX_PATH);

	// Set clear color
	glClearColor(0.f, 0.f, 0.f, 1.0f);
}

void LoadingGraphicsEngine::MainLoopEnd()
{
	CleanUp();

	calledMainLoopBegin = false;
}

void LoadingGraphicsEngine::ResizeCallback(GLFWwindow* window, int newWidth, int newHeight)
{
	// Set the viewport size. This is the only matrix that OpenGL maintains for us in modern OpenGL!
	glViewport(0, 0, windowWidth, windowHeight);
}

void LoadingGraphicsEngine::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

}

void LoadingGraphicsEngine::CursorPosCallback(GLFWwindow * window, double xpos, double ypos)
{

}

void LoadingGraphicsEngine::MouseWheelCallback(GLFWwindow * window, double xoffset, double yoffset)
{

}

void LoadingGraphicsEngine::MouseButtonCallback(GLFWwindow * window, int button, int action, int mods)
{

}

static glm::mat4 dotTransform(glm::vec3 position)
{
#define DOT_SCALE 0.025f
	return glm::scale(glm::translate(identifyMat, position), glm::vec3((DOT_SCALE * windowHeight) / windowWidth, DOT_SCALE, DOT_SCALE));
}

static glm::mat4 pawTransform(glm::vec3 position, float angle)
{
#define PAW_SCALE 0.125f
	glm::mat4 scale = glm::scale(glm::translate(identifyMat, position), glm::vec3((PAW_SCALE * windowHeight) / windowWidth, PAW_SCALE, PAW_SCALE));
	return glm::rotate(scale, angle + PAW_ANGLE_OFFSET, glm::vec3(0.f, 0.f, 1.f));
}

static void drawDot(glm::vec3 position, float alpha)
{
	glUniform1f(uAlpha, alpha);
	dotObj->Draw(passthroughShaderProgram, &identifyMat, &identifyMat, dotTransform(position));
}

static void drawPaw(glm::vec3 position, float alpha, float angle)
{
	glUniform1f(uAlpha, alpha);
	pawObj->Draw(passthroughShaderProgram, &identifyMat, &identifyMat, pawTransform(position, angle));
}

static bool isOffscreen(glm::vec3 p)
{
#define TOO_FAR (1.0f + PAW_SCALE)

	return (fabs(p.x) > TOO_FAR || fabs(p.y) > TOO_FAR);
}

void LoadingGraphicsEngine::MainLoopCallback(GLFWwindow * window)
{
	glViewport(0, 0, windowWidth, windowHeight);
	// Clear the color and depth buffers

	if (loadingAlpha >= 1.f) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glm::mat4 bgMat = glm::mat4(1.f);

	glUseProgram(passthroughShaderProgram);

	glUniform1f(uAlpha, loadingAlpha);
	backgroundObj->Draw(passthroughShaderProgram, &identifyMat, &identifyMat, bgMat);

	static const glm::vec3 dotPositions[] = {
		glm::vec3(0.34125f, 0.072f, 0.f),
		glm::vec3(0.40125f, 0.072f, 0.f),
		glm::vec3(0.46125f, 0.072f, 0.f),
	};

	static float dotAlphas[3] = {0.f};

#define DOT_PERIOD (1.5)
	double now = glfwGetTime();
	float dotPhase = (fmod(now, DOT_PERIOD) / DOT_PERIOD) * 4.0;

	switch((int)dotPhase) {
	case 0:
		dotAlphas[0] = dotPhase;
		dotAlphas[1] = 0.0f;
		dotAlphas[2] = 0.0f;
		break;
	case 1:
		dotAlphas[0] = 1.0f;
		dotAlphas[1] = dotPhase - 1.0f;
		dotAlphas[2] = 0.0f;
		break;
	case 2:
		dotAlphas[0] = 1.0f;
		dotAlphas[1] = 1.0f;
		dotAlphas[2] = dotPhase - 1.0f;
		break;
	case 3:
		dotAlphas[0] = 4.f - dotPhase;
		dotAlphas[1] = 4.f - dotPhase;
		dotAlphas[2] = 4.f - dotPhase;
		break;
	default:
		dotAlphas[0] = 0.f;
		dotAlphas[1] = 0.f;
		dotAlphas[2] = 0.f;
		break;
	}

	for (unsigned i = 0; i < 3; ++i) {
		drawDot(dotPositions[i], dotAlphas[i] * loadingAlpha);
	}
	
	static float pawAlphas[2] = { PAW_MAX_ALPHA, PAW_MAX_ALPHA };
	static float pawAngle = -PAW_ANGLE_OFFSET;
#define PAW_NUM_STARTING 3
	struct {
		glm::vec3 pos[2];
		float angle;
	} pawStartingPositions[] = {
		{{glm::vec3(0.275f, -1.15f, 0.f), glm::vec3(0.525f, -1.30f, 0.f)}, -PAW_ANGLE_OFFSET},
		{{glm::vec3(-1.15f, 0.5f, 0.f), glm::vec3(-1.425f, 0.65f, 0.f)}, -0.9f * glm::half_pi<float>()},
		{{glm::vec3(0.5f, -1.15f, 0.f), glm::vec3(0.65f, -1.425f, 0.f)}, 0},
	};
	static unsigned pawStartingIdx = 0;

	static glm::vec3 pawPos[2] = {
		glm::vec3(-0.5f, -0.5f, 0.f),
		glm::vec3(-0.25f, -0.65f, 0.f),
	};
	static glm::vec3 pawAnimationOffset[2] = {
		glm::vec3(0.f),
		glm::vec3(0.f),
	};
	static unsigned pawIdx = 1;
	static int pawPreviousPhaseIdx = 0;

	auto movementAngle = pawAngle + glm::half_pi<float>();
	glm::vec3 pawDirection(cosf(movementAngle), sinf(movementAngle), 0.f);

#define PAW_PERIOD 0.825
	float pawPhase = fmod(now, PAW_PERIOD) / PAW_PERIOD;

#define PAW_DOWN_FRAC 0.5
#define PAW_FADEOUT_FRAC 0.075
#define PAW_FADEIN_FRAC 0.075
#define PAW_UP_FRAC (1.0 - PAW_FADEOUT_FRAC - PAW_FADEIN_FRAC - PAW_DOWN_FRAC)
	int pawPhaseIdx;
	float pawSubphase;
	if (pawPhase < PAW_DOWN_FRAC) {
		pawSubphase = (pawPhase - 0.f) / PAW_DOWN_FRAC;
		pawPhaseIdx = 0;
	}
	else if (pawPhase < PAW_DOWN_FRAC + PAW_FADEOUT_FRAC) {
		pawSubphase = (pawPhase - PAW_DOWN_FRAC) / PAW_FADEOUT_FRAC;
		pawPhaseIdx = 1;
	}
	else if (pawPhase < PAW_DOWN_FRAC + PAW_FADEOUT_FRAC + PAW_UP_FRAC) {
		pawSubphase = (pawPhase - (PAW_DOWN_FRAC + PAW_FADEOUT_FRAC)) / PAW_UP_FRAC;
		pawPhaseIdx = 2;
	}
	else {
		pawSubphase = (pawPhase - (1.0 - PAW_FADEIN_FRAC)) / PAW_FADEIN_FRAC;
		pawPhaseIdx = 3;
	}

	/*
	 * Paw phases:
	 * 1. both paws down
	 * 2. paw i fades out and animates forward
	 * 3. paw i stays invisible
	 *    3a. if both paws offscreen, reposition
	 * 4. paw i fades in at new position
	 */
	pawAlphas[1] = pawAlphas[0] = PAW_MAX_ALPHA;
	pawAnimationOffset[1] = pawAnimationOffset[0] = glm::vec3(0.f);
	switch (pawPhaseIdx)
	{
	case 0:
		if (pawPreviousPhaseIdx) {
			pawIdx = !pawIdx;
		}
		break;
	case 1:
		pawAlphas[pawIdx] = (1.f - pawSubphase) * PAW_MAX_ALPHA;
		pawAnimationOffset[pawIdx] = pawSubphase * pawDirection * 0.015f;
		break;
	case 2:
		pawAlphas[pawIdx] = 0.f;
		break;
	case 3:
		if (pawPreviousPhaseIdx == 2) {
			pawPos[pawIdx] += pawDirection * 0.55f;
			if (isOffscreen(pawPos[0]) && isOffscreen(pawPos[1])) {
				pawStartingIdx = (pawStartingIdx + 1) % PAW_NUM_STARTING;
				pawPos[0] = pawStartingPositions[pawStartingIdx].pos[0];
				pawPos[1] = pawStartingPositions[pawStartingIdx].pos[1];
				pawAngle = pawStartingPositions[pawStartingIdx].angle;
				pawIdx = 0;
			}

			//cout << "Paw: <" << pawPos[pawIdx].x << ", " << pawPos[pawIdx].y << ">\n";
		}
		pawAlphas[pawIdx] = pawSubphase * PAW_MAX_ALPHA;
		pawAnimationOffset[pawIdx] = -(1.f - pawSubphase) * pawDirection * 0.015f;
		break;
	default:
		break;
	}

	pawPreviousPhaseIdx = pawPhaseIdx;

	drawPaw(pawPos[0] + pawAnimationOffset[0], pawAlphas[0] * sqrt(loadingAlpha), pawAngle);
	drawPaw(pawPos[1] + pawAnimationOffset[1], pawAlphas[1] * sqrt(loadingAlpha), pawAngle);
}

