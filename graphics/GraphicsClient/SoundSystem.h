#pragma once
#include <queue>
#include <mutex>
#include <thread>
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
private:
	FMOD::System * system;
	FMOD::Channel * channel[3];
	std::queue<Sound *> soundQueue;
	bool hasAudioDriver;
	bool continueQueue;
	std::mutex m;

public:
	SoundSystem();
	~SoundSystem();

	void createSoundEffect(Sound ** pSound, const char* pFile);
	void createBackgroundMusic(Sound ** pSound, const char* pFile);
	void playBackgroundMusic(Sound * pSound, bool bLoop = false);
	void playOtherPlayersSounds(Sound * pSound, bool bLoop = false);
	void playSoundEffect(Sound * pSound, bool bLoop = false);
	void pauseSoundEffect();
	void playSoundEffectNoOverlap(Sound * pSound, bool bLoop = false);
	void pauseAllSounds();
	void releaseSound(Sound * pSound);

	// This is integral in actually making the game work even if
	// there are no audio devices connected. If you don't wrap
	// sound code with this, it will complain that there aren't 
	// any audio drivers and crash the game.
	bool shouldIgnoreSound();

	void pushSoundQueue(Sound * pSound);
	void playSoundsInQueue();
	void playSoundsInQueueThread();
	void pauseSoundQueue();
};

