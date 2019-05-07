#include "AnimationChannel.h"
AnimationChannel::AnimationChannel(string boneName, int numKeyframes, Keyframe ** keyframes)
{
	this->boneName = boneName;
	//std::cerr << "Bone Name" << boneName << "\n";
	this->numKeyframes = numKeyframes;
	this->keyframes = keyframes;
	this->currKeyframe = 0;
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
		glm::mat4 scalingMatrix = glm::scale(glm::mat4(1.0f), keyframes[currKeyframe]->getScaling());
		glm::vec4 rotationQuat = keyframes[currKeyframe]->getRotation();
		float rotationAngle = acos(rotationQuat.w) * 2;
		glm::vec3 rotationAxis = glm::vec3(rotationQuat.x / sqrt(1 - (rotationQuat.w * rotationQuat.w)),
			rotationQuat.y / sqrt(1 - (rotationQuat.w * rotationQuat.w)),
			rotationQuat.z / sqrt(1 - (rotationQuat.w * rotationQuat.w)));
		glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotationAngle, rotationAxis);
		glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), keyframes[currKeyframe]->getPosition());
		//glm::mat4 newOffset = translationMatrix * rotationMatrix * scalingMatrix;
		glm::mat4 newOffset = scalingMatrix * rotationMatrix * translationMatrix;
		if (boneMap->find(boneName) != (boneMap)->end()) {
			(*boneMap)[boneName]->SetOffset(&newOffset);
		}
		else {
			std::cerr << boneName << "Bone nullptr\n";
		}
	}
}