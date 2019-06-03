#include "AnimationPlayer.h"



AnimationPlayer::AnimationPlayer(Skeleton * skel, Animation * anim)
{
	this->skeleton = skel;
	SetAnimation(anim);
	lastTime = Time::now();
	currTime = Time::now();
	durationFloat = 0.0f;
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
		currTime = (Time::now());
		fsec fs = currTime - lastTime;
		durationFloat += fs.count();
		lastTime = Time::now();
		if (durationFloat > animation->getEndTime())
			Reset();
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
		if (skeleton->GetBone(currName))
			skeleton->GetBone(currName)->SetChannel(currChannel);
		else
			std::cout << "ANIMPLAYER: NO BONE MATCHES THIS CHANNEL" << std::endl;
	}

	std::map<string, Bone *> * bones = skeleton->GetBones();
	for (std::map<string, Bone *>::iterator it = bones->begin(); it != bones->end(); it++) {
		Bone * currBone = it->second;
		if (currBone->GetName().find("joint") != string::npos && currBone->CheckIsBone() && currBone->GetChannel() == NULL) {
			std::cout << "JOINT WITH NO CHANNEL: " << currBone->GetName() << std::endl;
			currBone->CopyParentChannel();
			// TODO: channels.push_back(newChannel) would be required if we make a new channel rather than share the pointer
		}
	}
}

glm::mat4 AnimationPlayer::GetGlobalInverseT() {
	return animation->GetGlobalInverseT();
}

void AnimationPlayer::Reset() {
	this->animation->resetAnimation();
	durationFloat = 0.0f;
}