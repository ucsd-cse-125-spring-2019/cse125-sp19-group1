#ifndef AnimationChannel_h
#define AnimationChannel_h
#include "Keyframe.h"
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
	glm::mat4 transform;
public:
	AnimationChannel(string boneName, int numKeyframes, Keyframe ** keyframes);
	~AnimationChannel();

	void resetChannel();
	string getBoneName();
	int getNumKeyframes();
	Keyframe ** getKeyframes();
	void SetTransform(float currTime);
	glm::mat4 * GetTransform();

	void ToNextKeyframe();
};

#endif

