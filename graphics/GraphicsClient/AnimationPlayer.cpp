#include "AnimationPlayer.h"



AnimationPlayer::AnimationPlayer(Skeleton * skel, Animation * anim)
{
	this->skeleton = skel;
	SetAnimation(anim);
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
		animation->evaluateChannels(durationFloat);
		skeleton->Update(animation->GetGlobalInverseT());
		currTime = (Time::now());
		fsec fs = currTime - lastTime;
		durationFloat += fs.count();
		lastTime = Time::now();
		if (durationFloat > animation->getEndTime()) {
			std::cerr << "Reset of animation";
			animation->resetAnimation();
			durationFloat = 0;
		}
	}
}

void AnimationPlayer::setCurrTime(float time) {
	this->durationFloat = time;
}

void AnimationPlayer::SetAnimation(Animation * newAnimation) {
	animation = newAnimation;
	SetBoneChannels();
}

void AnimationPlayer::SetBoneChannels() {
	std::vector<AnimationChannel *> * channels = animation->GetChannels();
	for (int i = 0; i < channels->size(); i++) {
		AnimationChannel * currChannel = (*channels)[i];
		string currName = currChannel->getBoneName();
		skeleton->GetBone(currName)->SetChannel(currChannel);
	}
}
