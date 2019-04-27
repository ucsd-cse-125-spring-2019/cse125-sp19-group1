#ifndef AnimationPlayer_h
#define AnimationPlayer_h


#include "Animation.h"
#include "Skeleton.h"
class AnimationPlayer
{
private:
	Animation * animation;
	Skeleton * skeleton;
	float curr_time;
public:
	AnimationPlayer(Skeleton * skel, Animation * anim);
	~AnimationPlayer();
	void play();
	void setCurrTime(float new_time);
};

#endif

