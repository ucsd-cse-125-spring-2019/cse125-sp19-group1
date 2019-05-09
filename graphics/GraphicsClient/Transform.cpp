#include "Transform.h"

Transform::Transform(glm::mat4 mat) {
	offset = mat;
	this->type = type;
}
void Transform::addChild(Node * n) {
	children.push_back(n);
}


void Transform::draw(glm::mat4 V, glm::mat4 P, glm::mat4 model)
{
	glm::mat4 new_t = model * offset;
	for (int i = 0; i < children.size(); i++) {
		children[i]->draw(V, P, new_t);
	}
}

void Transform::setOffset(glm::mat4 newOffset) {
	offset = newOffset;
}