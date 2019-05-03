#include "Animation.h"

Animation::Animation(float startTime, float endTime, glm::mat4 * globalInverseTransform)
{
	this->startTime = startTime;
	this->endTime = endTime;
	this->globalInverseT = glm::mat4(*globalInverseTransform);
}


Animation::~Animation()
{
}

std::vector<AnimationChannel*> * Animation::GetChannels() {
	return &channels;
}

void Animation::resetAnimation() {
	for (int i = 0; i < channels.size(); i++) {
		channels[i]->resetChannel();
	}
}

float Animation::getStartTime() {
	return startTime;
}

float Animation::getEndTime() {
	return endTime;
}

/**
* For every channel, set the correct offsets for the time listed.
* Skeleton is needed to set the right bones
**/
void Animation::evaluateChannels(float currTime) {
	//std::cerr << "Printing numChannels" << numChannels << "\n";
	for (int i = 0; i < channels.size(); i++) {
		channels[i]->SetTransform(currTime);
	}
}

glm::mat4 * Animation::GetGlobalInverseT() {
	return &globalInverseT;
}
