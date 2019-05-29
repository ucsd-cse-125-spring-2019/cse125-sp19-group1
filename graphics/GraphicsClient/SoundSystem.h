#pragma once
#include "fmod.hpp"
#include "fmod.h"
//#include <windows.h>

typedef FMOD::Sound Sound;

/*
 * Channel 0: sound effects specific to yourself
 * Channel 1: sound effects specific to other players (3D)
 * Channel 2: background music
 */
class SoundSystem
{
public:
	FMOD::System * system;
	FMOD::Channel * channel[3];
	bool hasAudioDriver;
	SoundSystem();
	~SoundSystem();

	void createSoundEffect(Sound ** pSound, const char* pFile);
	void createBackgroundMusic(Sound ** pSound, const char* pFile);
	void playBackgroundMusic(Sound * pSound, bool bLoop = false);
	void playOtherPlayersSounds(Sound * pSound, bool bLoop = false);
	void playSoundEffect(Sound * pSound, bool bLoop = false);
	void playSoundEffectNoOverlap(Sound * pSound, bool bLoop = false);
	void releaseSound(Sound * pSound);
	bool shouldIgnoreSound();
};

