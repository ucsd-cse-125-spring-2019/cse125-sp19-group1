#include "AnimationChannel.h"
AnimationChannel::AnimationChannel(char * boneName, int numKeyframes, Keyframe ** keyframes)
{
	this->boneName = boneName;
	this->numKeyframes = numKeyframes;
	this->keyframes = keyframes;
	this->currKeyframe = 0;
}


AnimationChannel::~AnimationChannel()
{
}

const char * AnimationChannel::getBoneName() {
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
void AnimationChannel::setBoneOffset(float currTime, Skeleton * skel) {

	//switch keyframes if current time surpasses next keyframe
	if (currTime > keyframes[currKeyframe + 1]->getTime()) {
		if (currKeyframe + 1 > this->getNumKeyframes()) {
			currKeyframe = 0;
		}
		else {
			currKeyframe += 1;
		}
		std::map<string, Bone*> *boneMap = skel->GetBones();
		glm::mat4 newOffset = glm::scale(glm::mat4(), keyframes[currKeyframe]->getScaling());
		glm::vec4 rotationQuat = keyframes[currKeyframe]->getRotation();
		float rotationAngle = acos(rotationQuat.w) * 2;
		glm::vec3 rotationAxis = glm::vec3(rotationQuat.x / sqrt(1 - (rotationQuat.w * rotationQuat.w)),
			rotationQuat.y / sqrt(1 - (rotationQuat.w * rotationQuat.w)),
			rotationQuat.z / sqrt(1 - (rotationQuat.w * rotationQuat.w)));
		newOffset = glm::rotate(newOffset, rotationAngle, rotationAxis);
		newOffset = glm::translate(newOffset, keyframes[currKeyframe]->getPosition());
		if ((*boneMap)[boneName] != nullptr) {
		(*boneMap)[boneName]->SetOffset(&newOffset);
		}
		else {
			std::cerr << boneName << "Bone nullptr\n";
		}
	}
}