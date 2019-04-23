#include "Transform.h"

Transform::Transform(glm::mat4 mat) {
	transform = mat;
	this->type = type;
}
void Transform::addChild(Node * n) {
	children.push_back(n);
}


void Transform::draw(glm::mat4 V, glm::mat4 P)
{
	glm::mat4 new_t = V * transform;
	for (int i = 0; i < children.size(); i++) {
		children[i]->draw(new_t, P);
	}
}