#include "AnimationChannel.h"
AnimationChannel::AnimationChannel(string boneName, int numKeyframes, Keyframe ** keyframes)
{
	this->boneName = boneName;
	//std::cerr << "Bone Name" << boneName << "\n";
	this->numKeyframes = numKeyframes;
	this->keyframes = keyframes;
	this->currKeyframe = 0;
	this->transform = glm::mat4(1.0f);
}


AnimationChannel::~AnimationChannel()
{
}

void AnimationChannel::resetChannel() {
	currKeyframe = 0;
}

string AnimationChannel::getBoneName() {
	return boneName;
}

int AnimationChannel::getNumKeyframes() {
	return numKeyframes;
}

Keyframe ** AnimationChannel::getKeyframes() {
	return keyframes;
}

/**
* Method to alter the offset matrix of the bone with the same name as this channel.
* Should somehow interpolate value from Keyframe data.
* TODO:
* CURRENTLY THESE MATRICES DO ABSOLUTELY NOTHING (can replace w/identity and get same result)
**/
void AnimationChannel::SetTransform(float currTime) {
	//switch keyframes if current time surpasses next keyframe
	if (currTime > keyframes[currKeyframe + 1]->getTime()) {
		if (currKeyframe + 1 >= this->getNumKeyframes()) {
			currKeyframe = 0;
		}
		else {
			currKeyframe += 1;
		}
		glm::mat4 scalingMatrix = glm::scale(glm::mat4(1.0f), keyframes[currKeyframe]->getScaling());
		glm::vec4 rotationQuat = keyframes[currKeyframe]->getRotation();
		float rotationAngle = acos(rotationQuat.w) * 2;
		glm::vec3 rotationAxis = glm::vec3(rotationQuat.x / sqrt(1 - (rotationQuat.w * rotationQuat.w)),
			rotationQuat.y / sqrt(1 - (rotationQuat.w * rotationQuat.w)),
			rotationQuat.z / sqrt(1 - (rotationQuat.w * rotationQuat.w)));
		glm::mat4 rotationMatrix = rotate(glm::mat4(1.0f), rotationAngle, rotationAxis);
		glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), keyframes[currKeyframe]->getPosition());
		transform = translationMatrix * rotationMatrix * scalingMatrix;
	}
}

glm::mat4 * AnimationChannel::GetTransform() {
	return &transform;
}

void AnimationChannel::ToNextKeyframe() {
	if (currKeyframe + 1 >= this->getNumKeyframes())
		currKeyframe = 0;
	else
		currKeyframe += 1;

	glm::mat4 scalingMatrix = glm::scale(glm::mat4(1.0f), keyframes[currKeyframe]->getScaling());
	glm::vec4 rotationQuat = keyframes[currKeyframe]->getRotation();
	float rotationAngle = acos(rotationQuat.w) * 2;
	glm::vec3 rotationAxis = glm::vec3(rotationQuat.x / sqrt(1 - (rotationQuat.w * rotationQuat.w)),
		rotationQuat.y / sqrt(1 - (rotationQuat.w * rotationQuat.w)),
		rotationQuat.z / sqrt(1 - (rotationQuat.w * rotationQuat.w)));
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotationAngle, rotationAxis);
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), keyframes[currKeyframe]->getPosition());
	transform = translationMatrix * rotationMatrix * scalingMatrix;
}