#ifndef Animation_h
#define Animation_h

#include "AnimationChannel.h"
#include "Skeleton.h"
class Animation
{
private:
	std::vector<AnimationChannel*> channels;
	float startTime;
	float endTime;
	glm::mat4 globalInverseT;
public:
	Animation(float startTime, float endTime, glm::mat4 * globalInverseTransform);
	~Animation();
	std::vector<AnimationChannel*> * GetChannels();
	void resetAnimation();
	float getStartTime();
	float getEndTime();
	void evaluateChannels(float currTime);
	glm::mat4 * GetGlobalInverseT();
};

#endif
