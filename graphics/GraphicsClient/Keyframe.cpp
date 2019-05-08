#include "Keyframe.h"

Keyframe::Keyframe(float time, glm::vec3 position, glm::vec4 rotation, glm::vec3 scaling)
{
	this->time = time;
	this->position = position;
	this->rotation = rotation;
	this->scaling = scaling;
}


Keyframe::~Keyframe()
{
}

float Keyframe::getTime() {
	return time;
}

glm::vec3 Keyframe::getPosition() {
	return position;
}

glm::vec4 Keyframe::getRotation() {
	return rotation;
}

glm::vec3 Keyframe::getScaling() {
	return scaling;
}