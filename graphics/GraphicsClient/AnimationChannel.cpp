#include "AnimationChannel.h"
AnimationChannel::AnimationChannel(string boneName, int numKeyframes)
{
	this->boneName = boneName;
	//this->keyframes = keyframes;
	this->currKeyframe = 0;
  this->keyframes = std::vector<Keyframe *>();
}


AnimationChannel::~AnimationChannel()
{
  for (int i = 0; i < keyframes.size(); i++)
    delete keyframes[i];
}

void AnimationChannel::resetChannel() {
	currKeyframe = 0;
}

string AnimationChannel::getBoneName() {
	return boneName;
}

int AnimationChannel::getNumKeyframes() {
	return (int)keyframes.size();
}

std::vector<Keyframe *> * AnimationChannel::getKeyframes() {
	return &keyframes;
}

int AnimationChannel::GetCurrKeyframe() {
  return currKeyframe;
}

void AnimationChannel::SetCurrKeyframe(float currTime) {
	//switch keyframes if current time surpasses next keyframe
	if (currKeyframe + 1 < this->getNumKeyframes() && currTime > keyframes[currKeyframe + 1]->getTime())
		currKeyframe += 1;
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
