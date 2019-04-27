#include "AnimationPlayer.h"



AnimationPlayer::AnimationPlayer(Skeleton * skel, Animation * anim)
{
	this->skeleton = skel;
	this->animation = anim;

	//Debug Messages
	for (int i = 0; i < anim->getNumChannels(); i++) {
		std::cerr << (anim->getAnimationChannels())[i]->getBoneName() << "\n";
	}
}


AnimationPlayer::~AnimationPlayer()
{
}

void AnimationPlayer::play() {

}

void AnimationPlayer::setCurrTime(float time) {

}
