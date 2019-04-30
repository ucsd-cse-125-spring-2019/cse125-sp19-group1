#ifndef AnimationChannel_h
#define AnimationChannel_h
#include "Keyframe.h"
#include "Skeleton.h"
#include <string>
class AnimationChannel
{
private:
	string boneName;
	//note: setup assumes number of keyframes = 
	// numPositionKeys = numRotationKeys = numScalingKeys
	int numKeyframes;
	int currKeyframe;
	//keyframes will be in chronological order
	Keyframe ** keyframes;
public:
	AnimationChannel(string boneName, int numKeyframes, Keyframe ** keyframes);
	~AnimationChannel();

	void resetChannel();
	string getBoneName();
	int getNumKeyframes();
	Keyframe ** getKeyframes();
	void setBoneOffset(float currTime, Skeleton * skel);

};

#endif

