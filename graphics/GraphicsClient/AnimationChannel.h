#ifndef AnimationChannel_h
#define AnimationChannel_h
#include "Keyframe.h"
#include "Skeleton.h"
class AnimationChannel
{
private:
	char * boneName;
	//note: setup assumes number of keyframes = 
	// numPositionKeys = numRotationKeys = numScalingKeys
	int numKeyframes;
	//keyframes will be in chronological order
	Keyframe ** keyframes;
public:
	AnimationChannel(char * boneName, int numKeyframes, Keyframe ** keyframes);
	~AnimationChannel();

	const char * getBoneName();
	int getNumKeyframes();
	Keyframe ** getKeyframes();
	void setBoneOffset(float currTime, Skeleton * skel);

};

#endif

