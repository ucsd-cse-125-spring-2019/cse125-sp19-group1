#pragma once
#include <queue>
#include <map>
#include <thread>
#include <fmod_errors.h>
#include "fmod.hpp"
#include "fmod.h"
//#include <windows.h>

typedef FMOD::Sound Sound;

#define DISTANCE_FACTOR 3.0

// all the background loops will be here since we want to switch them here?


/*
 * 
 * Channel 0: sound effects specific to yourself
 * Channel 1: background music
 * Channel 2: keydrop channel (it shouldn't be paused)
 * ChannelGroup otherPlayerSounds: sounds produced by other players (not self)
 */
class SoundSystem
{
private:
	FMOD::System * system;
	FMOD::Channel * not3DChannel[3];
	FMOD::Channel * threeDeeChannel[4];
	FMOD::Channel * threeDeeSeparate;
	// int threeDeeChannelTaken;
	bool hasAudioDriver;
	bool continueQueue;

	Sound * backgroundSounds[7]; // FIXME: INCREDIBLY HARD-CODED
	int curLoop;

public:
	SoundSystem();
	~SoundSystem();

	// needs to be called every frame
	void setListenerLocation(float x, float y, float z);
	// needs to be called every frame for 3D calculations
	void update();

	void createSoundEffect(Sound ** pSound, const char* pFile);
	void createOtherPlayersSounds(Sound ** pSound, const char* pFile);

	void createBackgroundMusic(const char* pFile);
	void startBackgroundMusic();
	void nextBackgroundLoop(bool bLoop = false);
	void skipToLoop(int toLoop, bool isEndLoop = false);

	void playBackgroundMusic(Sound * pSound, bool bLoop = true);
	void playOtherPlayersSounds(Sound * pSound, int playerID, float x, float y, float z, bool playUntilEnd = false, bool bLoop = false);
	void playSoundEffect(Sound * pSound, bool playUntilEnd = false, bool bLoop = false);
	void pauseBackgroundMusic();
	void pauseOtherPlayersSounds(int playerID);
	void pauseSoundEffect();
	void playSoundEffectNoOverlap(Sound * pSound, bool bLoop = false);
	void pauseAllSounds();
	void releaseSound(Sound * pSound);

	// This is integral in actually making the game work even if
	// there are no audio devices connected. If you don't wrap
	// sound code with this, it will complain that there aren't 
	// any audio drivers and crash the game.
	bool shouldIgnoreSound();

};

