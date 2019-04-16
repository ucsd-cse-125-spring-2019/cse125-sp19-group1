#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <iostream>

#ifdef __APPLE__
// If modern OpenGL replace gl.h with gl3.h
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>
#else
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>
#include "Mesh.h"

class Window
{
public:
	static int width;
	static int height;
	static void initialize_objects();
	static void clean_up();
	static GLFWwindow* create_window(int width, int height);
	static void resize_callback(GLFWwindow* window, int width, int height);
	static void idle_callback();
	static void display_callback(GLFWwindow*);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void UpdateView();
	static glm::vec3 TrackballMapping(double x, double y, int width, int height);
	static void TrackballRotation(float rotationAngle, glm::vec3 rotationAxis);
	static void CursorPosCallback(GLFWwindow * window, double xpos, double ypos);
	static void MouseWheelCallback(GLFWwindow * window, double xoffset, double yoffset);
	static void MouseButtonCallback(GLFWwindow * window, int button, int action, int mods);
};

#endif
