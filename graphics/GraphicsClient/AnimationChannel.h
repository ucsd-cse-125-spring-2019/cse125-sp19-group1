#ifndef AnimationChannel_h
#define AnimationChannel_h
#include "Keyframe.h"
#include <glm/gtc/quaternion.hpp>
#include <string>
class AnimationChannel
{
private:
	string boneName;
	//note: setup assumes number of keyframes = 
	// numPositionKeys = numRotationKeys = numScalingKeys
	int currKeyframe;
	//keyframes will be in chronological order
  std::vector<Keyframe *> keyframes;
public:
  AnimationChannel(string boneName, int numKeyframes);
	~AnimationChannel();

	void resetChannel();
	string getBoneName();
	int getNumKeyframes();
  std::vector<Keyframe *> * getKeyframes();
  int GetCurrKeyframe();
	void SetCurrKeyframe(float currTime);
	void PrintMatrix(glm::mat4 * matrix);
};

#endif

