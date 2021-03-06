// client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <GLFW/glfw3.h>

#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <thread>

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "ERROR from GLFW: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	printf("INFO: got key %d scancode %d action %d\n", key, scancode, action);

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

static void fbsize_callback(GLFWwindow* window, int width, int height)
{
	printf("INFO: got GLFW framebuffer resize callback with new size %dx%d\n", width, height);

	// Or this can be done in the render loop
	glViewport(0, 0, width, height);
}

int client_main(GLFWwindow* window)
{
	while (!glfwWindowShouldClose(window))
	{
		// Keep running
		using namespace std::this_thread; // sleep_for, sleep_until
		using namespace std::chrono; // nanoseconds, system_clock, seconds

		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();

		sleep_for(nanoseconds(10));
	}

	return EXIT_SUCCESS;
}

int main()
{
	if (!glfwInit())
	{
		// Initialization failed
		fprintf(stderr, "ERROR: GLFW init failed\n");
		exit(EXIT_FAILURE);
	}

	glfwSetErrorCallback(error_callback);

	GLFWwindow* window = glfwCreateWindow(640, 480, "My Title", NULL, NULL);
	if (!window)
	{
		// Window or OpenGL context creation failed
		fprintf(stderr, "ERROR: GLFW window creation failed\n");
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	glfwSetFramebufferSizeCallback(window, fbsize_callback);

	int ret = client_main(window);

	glfwDestroyWindow(window);
	glfwTerminate();

    return ret;
}

