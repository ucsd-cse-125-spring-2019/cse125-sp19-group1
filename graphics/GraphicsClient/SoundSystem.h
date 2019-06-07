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

	Sound * backgroundSounds[2]; // FIXME: INCREDIBLY HARD-CODED
	/* 0: lobby loop
	 *		loop that plays during the lobby, play instructions_A after the loop ends 
	 * 1: instructions A
	 *		One-time song, stop after plays once, don't start InstructionsB until slide 3 comes in
	 * 2: instructions B
	 *		One-time song, play immediately once slide 3 comes in, transition to Loop A
	 * 3: loop a
	 *		Loop, transitions after song ends to loop B (in-game)
	 * 4: loop b
	 *		Loop, transitions after song ends to Transition C (in-game)
	 * 5: transition c
	 *		One-time, transitions after song ends to Ending_LOOP
	 * 6: ending loop
	 *		Loop until game ends
	 *		When the game ends, immediately cease music and start playing lobby loop.
	 */

public:
	SoundSystem();
	~SoundSystem();

	// needs to be called every frame
	void setListenerLocation(float x, float y, float z);
	// needs to be called every frame for 3D calculations
	void update();

	void createSoundEffect(Sound ** pSound, const char* pFile);
	void createOtherPlayersSounds(Sound ** pSound, const char* pFile);

	void createBackgroundMusic(int spot, const char* pFile);
	void startBackgroundMusic();
	void nextBackgroundLoop(int toPlay, bool bLoop = true);

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

