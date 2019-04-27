#ifndef Animation_h
#define Animation_h

#include "AnimationChannel.h"
#include "Skeleton.h"
class Animation
{
private:
	int numChannels;
	AnimationChannel** animationChannels;
	float startTime;
	float endTime;

public:
	Animation(int numChannels, AnimationChannel ** animationChannels, float startTime, float endTime);
	~Animation();
	float getStartTime();
	float getEndTime();
	int getNumChannels();
	AnimationChannel ** getAnimationChannels();
	void evaluateChannels(float currTime, Skeleton * skel);
};

#endif
