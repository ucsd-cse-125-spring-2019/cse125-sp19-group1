////////////////////////////////////////
// Tester.cpp
////////////////////////////////////////

#include "Tester.h"
#include "InGameGraphicsEngine.h"
#include "LoadingGraphicsEngine.h"
#include "../../network/ServerGame.h"
#include "../../network/ClientGame.h"

#include <cmath>

GLFWwindow * window = nullptr;
int windowWidth = 0;
int windowHeight = 0;
const char* window_title = GAME_NAME_SHORT;

static InGameGraphicsEngine * inGameEngine = nullptr;
static LoadingGraphicsEngine * loadingEngine = nullptr;
static AbstractGraphicsEngine * currentEngine = nullptr;
static ServerGame * server = nullptr;
static ClientGame * client = nullptr;

void ErrorCallback(int error, const char* description)
{
	// Print error
	fputs(description, stderr);
}

void SetupGlew()
{
	// Initialize GLEW. Not needed on OSX systems.
#ifndef __APPLE__
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		glfwTerminate();
	}
	fprintf(stdout, "Current GLEW version: %s\n", glewGetString(GLEW_VERSION));
#endif
}

void SetupOpenGLSettings()
{
#ifndef __APPLE__
	// Setup GLEW. Don't do this on OSX systems.
	SetupGlew();
#endif
	// Enable depth buffering
	glEnable(GL_DEPTH_TEST);
	// Related to shaders and z value comparisons for the depth buffer
	glDepthFunc(GL_LEQUAL);
	// Set polygon drawing mode to fill front and back of each polygon
	// You can also use the paramter of GL_LINE instead of GL_FILL to see wireframes
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// Disable backface culling to render both sides of polygons
	glDisable(GL_CULL_FACE);
	/* Enable culling of faces to speed up rendering
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK); */
	// Set shading to smooth
	glShadeModel(GL_SMOOTH);
	// Auto normalize surface normals
	glEnable(GL_NORMALIZE);
	// Enable the 4x multisampling that we asked GLFW for
	glEnable(GL_MULTISAMPLE);
}

void PrintVersions()
{
	// Get info of GPU and supported OpenGL version
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("OpenGL version supported %s\n", glGetString(GL_VERSION));

	//If the shading language symbol is defined
#ifdef GL_SHADING_LANGUAGE_VERSION
	std::printf("Supported GLSL version is %s.\n", (char *)glGetString(GL_SHADING_LANGUAGE_VERSION));
#endif
}

void Init()
{
	server = new ServerGame();
	client = new ClientGame();
	//_beginthread(serverLoop, 0, (void*)12);

	inGameEngine = new InGameGraphicsEngine(client);
	loadingEngine = new LoadingGraphicsEngine();
	currentEngine = loadingEngine;

	inGameEngine->StartLoading();
	loadingEngine->StartLoading();
}

void serverLoop(void * args) {
	while (true) {
		server->update();
	}
}

// Treat this as a destructor function. Delete dynamically allocated memory here.
void CleanUp() {
	currentEngine = nullptr;

	if (inGameEngine) {
		inGameEngine->CleanUp();
		delete inGameEngine;
		inGameEngine = nullptr;
	}

	if (loadingEngine) {
		loadingEngine->CleanUp();
		delete loadingEngine;
		loadingEngine = nullptr;
	}
}

void ResizeCallback(GLFWwindow* window, int newWidth, int newHeight)
{
#ifdef __APPLE__
	glfwGetFramebufferSize(window, &newWidth, &newHeight); // In case your Mac has a retina display
#endif
	windowWidth = newWidth;
	windowHeight = newHeight;

	if (currentEngine)
		currentEngine->ResizeCallback(window, newWidth, newHeight);
}

GLFWwindow* CreateWindowFrame(int width, int height)
{
	// Initialize GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return NULL;
	}

	// 4x antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ // Because Apple hates comforming to standards
	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window
	GLFWwindow* window = glfwCreateWindow(width, height, window_title, NULL, NULL);

	// Check if the window could not be created
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		fprintf(stderr, "Either GLFW is not installed or your graphics card does not support modern OpenGL.\n");
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window
	glfwMakeContextCurrent(window);

	// Set swap interval to 1
	glfwSwapInterval(1);

	// Get the width and height of the framebuffer to properly resize the window
	glfwGetFramebufferSize(window, &width, &height);
	// Call the resize callback to make sure things get drawn immediately
	ResizeCallback(window, width, height);

	return window;
}



void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (currentEngine && currentEngine->fullyLoaded)
		currentEngine->KeyCallback(window, key, scancode, action, mods);
}

void CursorPosCallback(GLFWwindow * window, double xpos, double ypos) {
	if (currentEngine && currentEngine->fullyLoaded)
		currentEngine->CursorPosCallback(window, xpos, ypos);
}

void MouseWheelCallback(GLFWwindow * window, double xoffset, double yoffset) {
	if (currentEngine && currentEngine->fullyLoaded)
		currentEngine->MouseWheelCallback(window, xoffset, yoffset);
}

void MouseButtonCallback(GLFWwindow * window, int button, int action, int mods) {
	if (currentEngine && currentEngine->fullyLoaded)
		currentEngine->MouseButtonCallback(window, button, action, mods);
}

void SetupCallbacks()
{
	// Set the error callback
	glfwSetErrorCallback(ErrorCallback);
	// Set the key callback
	glfwSetKeyCallback(window, KeyCallback);
	// Set the window resize callback
	glfwSetFramebufferSizeCallback(window, ResizeCallback);
	// Set the mouse input callback
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	// Set the cursor position callback
	glfwSetCursorPosCallback(window, CursorPosCallback);
	// Set the mouse wheel callback
	glfwSetScrollCallback(window, MouseWheelCallback);
}

int main(void)
{
	// Create the GLFW window
	window = CreateWindowFrame(800, 600);
	// Print OpenGL and GLSL versions
	PrintVersions();
	// Setup callbacks
	SetupCallbacks();
	// Setup OpenGL settings, including lighting, materials, etc.
	SetupOpenGLSettings();
	// Initialize objects/pointers for rendering
	Init();

	double loadingFadeoutStart;

	// Loop while GLFW window should stay open
	while (!glfwWindowShouldClose(window))
	{
		using namespace std::chrono;

		auto start = high_resolution_clock::now();

		if (currentEngine == loadingEngine) {
			if (inGameEngine->fullyLoaded) {
				currentEngine = inGameEngine;
				loadingFadeoutStart = glfwGetTime();
			}
		}
		else if (loadingEngine) {
#define LOADING_FADEOUT_TIME 1.35
			double delta = glfwGetTime() - loadingFadeoutStart;
			if (delta < LOADING_FADEOUT_TIME) {
				loadingEngine->loadingAlpha = 1.f - delta / LOADING_FADEOUT_TIME;
			}
			else {
				loadingEngine->loadingAlpha = 0.f;
				loadingEngine->MainLoopEnd();
				delete loadingEngine;
				loadingEngine = nullptr;
			}
		}
		
		if (currentEngine && currentEngine->fullyLoaded) {
			if (!currentEngine->calledMainLoopBegin) {
				currentEngine->ResizeCallback(window, windowWidth, windowHeight);
				currentEngine->MainLoopBegin();
			}
			currentEngine->MainLoopCallback(window);

			if (loadingEngine && currentEngine != loadingEngine) {
				loadingEngine->MainLoopCallback(window);
			}

			// Swap buffers
			glfwSwapBuffers(window);
			// Gets events, including input such as keyboard and mouse or window resizing
			glfwPollEvents();
		}
		
		server->update();

		auto finish = high_resolution_clock::now();

		// Limit to no more than 60fps
		auto frameTime = duration_cast<microseconds>(finish - start);
		if (frameTime < chrono::microseconds(16666)) {
			this_thread::sleep_for(chrono::microseconds(16666) - frameTime);
		}
		
		//cout << frameTime.count() / 1000.0 << endl;   // print # of milliseconds it took to process this frame
	}

	CleanUp();
	// Destroy the window
	glfwDestroyWindow(window);
	// Terminate GLFW
	glfwTerminate();

	return EXIT_SUCCESS;
}
