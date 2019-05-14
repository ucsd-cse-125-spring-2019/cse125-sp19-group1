#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#define GLFW_INCLUDE_GLEXT
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Node.h"
#include "FBXObject.h"

class Geometry : public Node
{
public:
	FBXObject * fbx;
	glm::mat4 t;
	GLint shaderProgram;
	int refCount;
	Geometry(FBXObject * fbx, GLint shaderProgram);
	//~Geometry();
	void draw(glm::mat4 V, glm::mat4 P, glm::mat4 model);
	void addChild(Node * child);
	void removeAllChildren();
	bool decrementRefCount();
	void incrementRefCount();
	//void draw(glm::mat4 C, float alpha);
};
#endif