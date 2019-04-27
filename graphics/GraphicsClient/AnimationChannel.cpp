#include "AnimationChannel.h"
AnimationChannel::AnimationChannel(char * boneName, int numKeyframes, Keyframe ** keyframes)
{
	this->boneName = boneName;
	this->numKeyframes = numKeyframes;
	this->keyframes = keyframes;
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

}