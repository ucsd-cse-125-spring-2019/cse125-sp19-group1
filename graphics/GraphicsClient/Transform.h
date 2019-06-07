#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp> 
#include <stdlib.h>
#include <stdio.h>
#include "node.h"
#include <vector>


class Transform : public Node
{
public:
	bool hidden;

private:
	glm::mat4 offset;
	bool type;
	int refCount;
	std::vector<Node*> children;
public:
	Transform(glm::mat4 mat);
	void addChild(Node * n);
	void removeAllChildren();
	void draw(const glm::mat4 &V, const glm::mat4 &P, glm::mat4 model);
	const glm::mat4 &getOffset() const;
	void setOffset(glm::mat4);
	bool decrementRefCount();
	void incrementRefCount();
	void applyScale(glm::vec3 scale);
	void undoScale();
};
#endif