#ifndef _NODE_H_
#define _NODE_H_
#include <glm/mat4x4.hpp>

class Node
{
public:
	Node();
	virtual void draw(const glm::mat4 &V, const glm::mat4 &P, glm::mat4 model) = 0;
	virtual void addChild(Node * child) = 0;
	virtual void removeAllChildren() = 0;
	virtual bool decrementRefCount() = 0;
	virtual void incrementRefCount() = 0;
};

#endif