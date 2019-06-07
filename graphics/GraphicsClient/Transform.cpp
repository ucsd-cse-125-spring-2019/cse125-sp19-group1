#include "Transform.h"

Transform::Transform(glm::mat4 mat) {
	offset = mat;
	refCount = 1;
	this->type = type;
	hidden = false;
}

void Transform::addChild(Node * n) {
	children.push_back(n);
}

void Transform::removeAllChildren() {
	children.clear();
}

void Transform::draw(const glm::mat4 &V, const glm::mat4 &P, glm::mat4 model)
{
	if (!hidden) {
		glm::mat4 new_t = model * offset;
		for (int i = 0; i < children.size(); i++) {
			children[i]->draw(V, P, new_t);
		}
	}
}

const glm::mat4 &Transform::getOffset() const {
	return offset;
}

void Transform::setOffset(glm::mat4 newOffset) {
	offset = newOffset;
}

bool Transform::decrementRefCount()
{
	return (--refCount <= 0);
}

void Transform::incrementRefCount()
{
	++refCount;
}

void Transform::applyScale(glm::vec3 scale) {
	offset = glm::scale(offset, scale);
}

void Transform::undoScale() {
	offset[0] = glm::normalize(glm::vec4(offset[0][0], offset[0][1], offset[0][2], offset[0][3]));
	offset[1] = glm::normalize(glm::vec4(offset[1][0], offset[1][1], offset[1][2], offset[1][3]));
	offset[2] = glm::normalize(glm::vec4(offset[2][0], offset[2][1], offset[2][2], offset[2][3]));
}
