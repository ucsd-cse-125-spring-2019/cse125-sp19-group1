#include "Animation.h"

Animation::Animation(int numChannels, AnimationChannel** animationChannels, float startTime, float endTime)
{
	this->numChannels = numChannels;
	this->animationChannels = animationChannels;
	this->startTime = startTime;
	this->endTime = endTime;
}


Animation::~Animation()
{
}

float Animation::getStartTime() {
	return startTime;
}

float Animation::getEndTime() {
	return endTime;
}
int Animation::getNumChannels() {
	return numChannels;
}

/**
* For every channel, set the correct offsets for the time listed.
* Skeleton is needed to set the right bones
**/
void Animation::evaluateChannels(float currTime, Skeleton * skel) {
	for (int i = 0; i < numChannels; i++) {
		animationChannels[i]->setBoneOffset(currTime, skel);
	}
}

AnimationChannel** Animation::getAnimationChannels() {
	return animationChannels;
}
