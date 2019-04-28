#include "AnimationPlayer.h"



AnimationPlayer::AnimationPlayer(Skeleton * skel, Animation * anim)
{
	this->skeleton = skel;
	this->animation = anim;
	lastTime = Time::now();
	currTime = Time::now();
	durationFloat = 0;
	//Debug Messages
	/*for (int i = 0; i < anim->getNumChannels(); i++) {
		std::cerr << (anim->getAnimationChannels())[i]->getBoneName() << "\n";
		std::cerr << (anim->getAnimationChannels())[i]->getNumKeyframes() << "Keyframes in Channel\n";
	}*/
}


AnimationPlayer::~AnimationPlayer()
{
	delete animation;
}

void AnimationPlayer::play() {
	//std::cerr << durationFloat << "Timer value\n";
	if (animation != NULL) {
		animation->evaluateChannels(durationFloat, skeleton);
		currTime = (Time::now());
		fsec fs = currTime - lastTime;
		durationFloat += fs.count();
		lastTime = Time::now();
		if (durationFloat > animation->getEndTime()) {
			std::cerr << "Reset of animation";
			durationFloat = 0;
		}
	}
}

void AnimationPlayer::setCurrTime(float time) {
	this->durationFloat = time;
}
