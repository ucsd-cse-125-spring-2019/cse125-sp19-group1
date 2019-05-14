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
		Keyframe * key = keyframes[currKeyframe];
		glm::mat4 scalingMatrix = glm::scale(glm::mat4(1.0f), key->getScaling());
		glm::quat rotationQuat = glm::quat(key->getRotation());
		glm::mat4 rotationMatrix = rotationQuat.operator glm::mat<4, 4, float, glm::packed_highp>();
		glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), key->getPosition());
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

	Keyframe * key = keyframes[currKeyframe];
	glm::mat4 scalingMatrix = glm::scale(glm::mat4(1.0f), key->getScaling());
	glm::quat rotationQuat = glm::quat(key->getRotation());
	glm::mat4 rotationMatrix = rotationQuat.operator glm::mat<4, 4, float, glm::packed_highp>();
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), key->getPosition());
	transform = translationMatrix * rotationMatrix * scalingMatrix;
}

void AnimationChannel::PrintMatrix(glm::mat4 * matrix) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			std::cerr << (*matrix)[i][j] << " ";
		}
		std::cerr << std::endl;
	}
	std::cerr << std::endl;
}

int AnimationChannel::GetCurrKeyframe() {
	return currKeyframe;
}