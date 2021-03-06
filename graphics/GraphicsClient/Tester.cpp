////////////////////////////////////////
// Tester.cpp
////////////////////////////////////////

#define _CRT_SECURE_NO_WARNINGS

#include "Tester.h"
#include "InGameGraphicsEngine.h"
#include "LoadingGraphicsEngine.h"
#include "LobbyGraphicsEngine.h"
#include "CutsceneGraphicsEngine.h"
#include "PlayAgainGraphicsEngine.h"

#include "../../network/ServerGame.h"
#include "../../network/ClientGame.h"

#include <cmath>

GLFWwindow * window = nullptr;
int windowWidth = 0;
int windowHeight = 0;
const char* window_title = GAME_NAME_SHORT;

static InGameGraphicsEngine * inGameEngine = nullptr;
static LoadingGraphicsEngine * loadingEngine = nullptr;
static LobbyGraphicsEngine * lobbyEngine = nullptr;

vector<CutsceneGraphicsEngine *> startingCutscenes;
CutsceneGraphicsEngine *chefWinsCutscene = nullptr;
CutsceneGraphicsEngine *animalsWinCutscene = nullptr;
PlayAgainGraphicsEngine *playAgainEngine = nullptr;

static AbstractGraphicsEngine * currentEngine = nullptr;
static AbstractGraphicsEngine * previousEngine = nullptr;  // for crossfading

ClientGame * sharedClient = nullptr;

//#define DEBUG_CLIENTS
#ifdef DEBUG_CLIENTS
static ServerGame* server = nullptr;
static ClientGame * clients[4] = { nullptr };

void toggleClient() {
	for (int i = 0; i < 4; i++) {
		if (sharedClient == clients[i]) {
			sharedClient = clients[(i + 1) % 4];
			break;
		}
	}
}
#else
void toggleClient() {}
#endif

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

void Init(GLFWwindow *window)
{
	loadingEngine = new LoadingGraphicsEngine();
	loadingEngine->StartLoading();

	// Display a basic loading screen, better than blank :(
	loadingEngine->MainLoopBegin();
	loadingEngine->temporarilySuppressAnimation = true;
	loadingEngine->MainLoopCallback(window);
	glfwSwapBuffers(window);
	glfwPollEvents();

#ifdef DEBUG_CLIENTS
	server = new ServerGame();

	for (auto &client : clients) {
		client = new ClientGame();
	}
	sharedClient = clients[0];
#else
	sharedClient = new ClientGame();
#endif

	lobbyEngine = new LobbyGraphicsEngine();
	lobbyEngine->StartLoading();

	// Now that we have something on-screen, make sure the first screen loads before slow shader compilation
	sharedClient->update();
	if (sharedClient->getGameData()->getGameState() == GameState::IN_GAME) {
		currentEngine = loadingEngine;
	}
	else {
		currentEngine = lobbyEngine;

		// In this order so that the reference counted passthrough shader won't get recompiled
		lobbyEngine->MainLoopBegin();
		loadingEngine->MainLoopEnd();
	}

	inGameEngine = new InGameGraphicsEngine();

#define CUTSCENES_DIR "../2D Elements/"
#define CUTSCENE_FILE(x) CUTSCENES_DIR "cutscene - " x ".png"
	startingCutscenes.push_back(new CutsceneGraphicsEngine(CUTSCENE_FILE("cake1_2")));
	startingCutscenes.push_back(new CutsceneGraphicsEngine(CUTSCENE_FILE("cake2_2")));
	startingCutscenes.push_back(new CutsceneGraphicsEngine(CUTSCENE_FILE("cake6_2")));
	startingCutscenes.push_back(new CutsceneGraphicsEngine(CUTSCENE_FILE("instructions")));
	startingCutscenes.push_back(new CutsceneGraphicsEngine(CUTSCENE_FILE("exits 1")));
	startingCutscenes.push_back(new CutsceneGraphicsEngine(CUTSCENE_FILE("exits 2")));
	startingCutscenes.push_back(new CutsceneGraphicsEngine(CUTSCENE_FILE("exits 3")));
	startingCutscenes.push_back(new CutsceneGraphicsEngine(CUTSCENE_FILE("angry meter explanation")));

	chefWinsCutscene = new CutsceneGraphicsEngine(CUTSCENE_FILE("chef win"));
	animalsWinCutscene = new CutsceneGraphicsEngine(CUTSCENE_FILE("animal win"));
	
	//chefWinsCutscene->showSkippableMsg = false;
	//animalsWinCutscene->showSkippableMsg = false;
	
	playAgainEngine = new PlayAgainGraphicsEngine();

	chefWinsCutscene->StartLoading();
	animalsWinCutscene->StartLoading();
	playAgainEngine->StartLoading();

	for (auto cutscene : startingCutscenes) {
		cutscene->StartLoading();
	}

	inGameEngine->StartLoading();
}

// Treat this as a destructor function. Delete dynamically allocated memory here.
void CleanUp() {
	currentEngine = nullptr;

	/*if (inGameEngine) {
		inGameEngine->CleanUp();
		delete inGameEngine;
		inGameEngine = nullptr;
	}*/

	if (loadingEngine) {
		loadingEngine->CleanUp();
		delete loadingEngine;
		loadingEngine = nullptr;
	}

	if (lobbyEngine) {
		lobbyEngine->CleanUp();
		delete lobbyEngine;
		lobbyEngine = nullptr;
	}

	if (chefWinsCutscene) {
		chefWinsCutscene->CleanUp();
		delete chefWinsCutscene;
		chefWinsCutscene = nullptr;
	}

	if (animalsWinCutscene) {
		animalsWinCutscene->CleanUp();
		delete animalsWinCutscene;
		animalsWinCutscene = nullptr;
	}

	if (playAgainEngine) {
		playAgainEngine->CleanUp();
		delete playAgainEngine;
		playAgainEngine = nullptr;
	}

	for (auto cutscene : startingCutscenes) {
		delete cutscene;
	}
	startingCutscenes.clear();

#ifdef DEBUG_CLIENTS
	delete server;
#endif

	delete sharedClient;
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
#ifndef DEBUG_CLIENTS
	cout << "Enter an IP address to connect to [or leave blank to connect to localhost]: ";
	string newIp;
	getline(cin, newIp);
	if (newIp.size()) {
		strncpy(serverIpAddress, newIp.c_str(), sizeof(serverIpAddress));
		serverIpAddress[sizeof(serverIpAddress) - 1] = '\0';
	}
#endif

	// Create the GLFW window
	window = CreateWindowFrame(1600, 900);
	// Print OpenGL and GLSL versions
	PrintVersions();
	// Setup callbacks
	SetupCallbacks();
	// Setup OpenGL settings, including lighting, materials, etc.
	SetupOpenGLSettings();
	// Initialize objects/pointers for rendering
	Init(window);

	double crossfadeStart;

	// Loop while GLFW window should stay open
	while (!glfwWindowShouldClose(window))
	{
		using namespace std::chrono;

		auto start = high_resolution_clock::now();

		AbstractGraphicsEngine *targetEngine = nullptr;

		if (previousEngine) {
#define CROSSFADE_DURATION 0.5
			double delta = glfwGetTime() - crossfadeStart;
			if (delta < CROSSFADE_DURATION) {
				if (previousEngine != inGameEngine) {
					previousEngine->screenAlpha = tanh((1.0 - delta / CROSSFADE_DURATION) * 5.0 - 2.5) * 0.5 + 0.5;
				}
				else {
					currentEngine->screenAlpha = tanh((delta / CROSSFADE_DURATION) * 5.0 - 2.5) * 0.5 + 0.5;
				}
			}
			else {
				if (previousEngine != inGameEngine) {
					previousEngine->screenAlpha = 0.f;
				}
				currentEngine->screenAlpha = 1.f;

				previousEngine->MainLoopEnd();
				previousEngine = nullptr;
			}
		} 
		else if (currentEngine == loadingEngine) {
#ifdef DEBUG_CLIENTS
			if (inGameEngine->fullyLoaded && sharedClient->gameData->getGameState() == GameState::IN_GAME) {
				targetEngine = inGameEngine;
			}
#else
			if (inGameEngine->fullyLoaded && sharedClient->gameData->getAllPlayersLoaded()) {
				targetEngine = inGameEngine;
			}
#endif
		}
		
		if (currentEngine->ShouldFadeout()) {
			if (currentEngine == lobbyEngine) {
				targetEngine = startingCutscenes[0];
			}
			else if (currentEngine == chefWinsCutscene || currentEngine == animalsWinCutscene) {
				targetEngine = playAgainEngine;
			}
			else if (currentEngine == inGameEngine) {
				switch (sharedClient->getGameData()->getWT()) {
				case WinType::CHEF_WIN:
					targetEngine = chefWinsCutscene;	break;
				case WinType::DOOR:
				case WinType::TOILET:
				case WinType::VENT:
					targetEngine = animalsWinCutscene;	break;
				default:
					targetEngine = playAgainEngine;		break;
				}
			}
			else if (currentEngine == playAgainEngine) {
				targetEngine = lobbyEngine;
			}
			else {
				for (unsigned i = 0; i < startingCutscenes.size(); ++i) {
					auto engine = startingCutscenes[i];
					if (currentEngine == engine) {
						if (i + 1 < startingCutscenes.size() && !engine->shouldSkipTheRest()) {
							targetEngine = startingCutscenes[i + 1];
						}
						else {
							targetEngine = (inGameEngine->fullyLoaded) && sharedClient->gameData->getAllPlayersLoaded() ? (AbstractGraphicsEngine*)inGameEngine : (AbstractGraphicsEngine*)loadingEngine;
						}
						break;
					}
				}
			}
		}

		if (targetEngine) {
			if (previousEngine) {
				previousEngine->MainLoopEnd();
			}

			previousEngine = currentEngine;
			currentEngine = targetEngine;
			crossfadeStart = glfwGetTime();
		}

		if (currentEngine && currentEngine->fullyLoaded) {
			if (!currentEngine->calledMainLoopBegin) {
				currentEngine->ResizeCallback(window, windowWidth, windowHeight);
				currentEngine->MainLoopBegin();

				// start over on the crossfade because MainLoopBeing() may have taken significant time
				crossfadeStart = glfwGetTime();
				if (previousEngine) 
				{
					previousEngine->screenAlpha = 1.f;
					currentEngine->screenAlpha = (float)(previousEngine != inGameEngine);
				}
			}

			if (previousEngine && previousEngine == inGameEngine) {
				previousEngine->MainLoopCallback(window);
			}

			currentEngine->MainLoopCallback(window);

			if (previousEngine && previousEngine != inGameEngine) {
				previousEngine->MainLoopCallback(window);
			}

			// Swap buffers
			glfwSwapBuffers(window);
			// Gets events, including input such as keyboard and mouse or window resizing
			glfwPollEvents();
		}
		
#ifdef DEBUG_CLIENTS
		server->update();
#endif

		// Only update client from tester.cpp while game is loading
		if (sharedClient->gameData->getGameState() == GameState::LOADING) {
			sharedClient->update();

			// Send done loading event only if client is on loading screen, game state = loading, and in game engine is fully loaded
			if (inGameEngine->fullyLoaded && currentEngine == (AbstractGraphicsEngine*)loadingEngine)
			{
				// Limit to no more often that once per second
				static double lastSent = -1000.0;
				double now = glfwGetTime();
				if (now > lastSent + 1.0) {
					lastSent = now;

#ifdef DEBUG_CLIENTS
					sharedClient->sendPackets(SET_DONE_LOADING_EVENT);
#else
					sharedClient->sendPackets(DONE_LOADING_EVENT);
#endif
				}
			}
		}

		auto finish = high_resolution_clock::now();

		// Limit to no more than 60fps
		auto frameTime = duration_cast<microseconds>(finish - start);
		if (frameTime < chrono::microseconds(16666)) {
			this_thread::sleep_for(chrono::microseconds(16666) - frameTime);
		}
		
		//cout << "Frame milliseconds: " << frameTime.count() / 1000.0 << endl;   // print # of milliseconds it took to process this frame
	}

	CleanUp();
	// Destroy the window
	glfwDestroyWindow(window);
	// Terminate GLFW
	glfwTerminate();
	glfwTerminate();
	glfwTerminate();

	return EXIT_SUCCESS;
}
