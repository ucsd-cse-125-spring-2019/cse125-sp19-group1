#include "LoadingGraphicsEngine.h"



LoadingGraphicsEngine::LoadingGraphicsEngine()
{

}

LoadingGraphicsEngine::~LoadingGraphicsEngine()
{

}

void LoadingGraphicsEngine::StartLoading()  // may launch a thread and return immediately
{

}

void LoadingGraphicsEngine::CleanUp()
{

}

void LoadingGraphicsEngine::MainLoopBegin()
{

}

void LoadingGraphicsEngine::MainLoopEnd()
{

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

void LoadingGraphicsEngine::MainLoopCallback(GLFWwindow * window)
{
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glViewport(0, 0, windowWidth, windowHeight);

	//TODO...

	// Swap buffers
	glfwSwapBuffers(window);
	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
}

