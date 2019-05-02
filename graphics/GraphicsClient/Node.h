#ifndef _NODE_H_
#define _NODE_H_
#include <glm/mat4x4.hpp>

class Node
{
public:
	Node();
	virtual void draw(glm::mat4 V, glm::mat4 P) = 0;
	virtual void addChild(Node * child) = 0;
};

#endif