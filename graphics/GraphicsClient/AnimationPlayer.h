#ifndef AnimationPlayer_h
#define AnimationPlayer_h


#include "Animation.h"
#include "Skeleton.h"
#include <chrono>
typedef std::chrono::high_resolution_clock Time;
typedef std::chrono::duration<float> fsec;

class AnimationPlayer
{
private:
	Animation * animation;
	Skeleton * skeleton;
	std::chrono::steady_clock::time_point currTime;
	std::chrono::steady_clock::time_point lastTime;
	float durationFloat;
public:
	AnimationPlayer(Skeleton * skel, Animation * anim);
	~AnimationPlayer();
	void play();
	void setCurrTime(float new_time);
	void SetAnimation(Animation * newAnimation);
	void SetBoneChannels();
};

#endif

