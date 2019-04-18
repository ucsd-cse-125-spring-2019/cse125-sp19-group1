#ifndef _CUBE_H_
#define _CUBE_H_

#ifdef __APPLE__
// If modern OpenGL replace gl.h with gl3.h
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#else
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Cube
{
public:
	Cube(float);
	~Cube();

	glm::mat4 toWorld;

	float size;
	float angle;

	void draw();
	void update();

	void spin(float);
};

#endif

