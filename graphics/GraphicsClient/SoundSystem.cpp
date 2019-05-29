#include "SoundSystem.h"
#include <iostream>
#include <sys/stat.h>

#define NULL 0

SoundSystem::SoundSystem()
{
	FMOD_RESULT result;

	if (FMOD::System_Create(&system) != FMOD_OK) {
		// Report Error
		return;
	}

	int driverCount = 0;
	system->getNumDrivers(&driverCount);
	channel[0] = 0;
	channel[1] = 0;
	channel[2] = 0;

	if (driverCount == 0) {
		fprintf(stdout, "SoundSystem ERROR: driverCount = 0, possibly because no audio devices are plugged in\n");
		hasAudioDriver = false;
		return;
	}
	else {
		hasAudioDriver = true;
		fprintf(stdout, "SoundSystem: Number of Drivers=%d\n", driverCount);
	}

	result = system->setDriver(NULL);
	if (result != FMOD_OK) {
		fprintf(stdout, "SoundSystem ERROR: setDriver\n");
	}

	result = system->init(32, FMOD_INIT_NORMAL, NULL);
	if (result != FMOD_OK) {
		fprintf(stdout, "SoundSystem ERROR %d: CANNOT INITIALIZE SOUNDSYSTEM\n", result);
	}
}


SoundSystem::~SoundSystem()
{
}

void SoundSystem::createSoundEffect(Sound ** pSound, const char* pFile)
{
	FMOD_RESULT result;
	struct stat buffer;

	// check if file exists
	if (stat(pFile, &buffer) != 0) {
		fprintf(stdout, "createSound ERROR: FILE DOES NOT EXIST\n");
	}

	fprintf(stdout, "createSound: before system->createSound pSound=%d\n", pSound);
	// can also use FMOD_CREATESAMPLE to load entire sound
	// and decompress it in memory to speed up playback
	result = system->createSound(pFile, FMOD_DEFAULT, 0, pSound);
	if (result != FMOD_OK) {
		if (result == FMOD_ERR_UNINITIALIZED) {
			fprintf(stdout, "createSound ERROR: FMOD_ERR_UNINITIALIZED\n");
		}
		else if (result == FMOD_ERR_UNSUPPORTED) {
			fprintf(stdout, "createSound ERROR: FMOD_ERR_UNSUPPORTED\n");
		}
		else {
			fprintf(stdout, "createSound ERROR %d: COULD NOT CREATE SOUND %s\n", result, pFile);
		}
	}
	else {
		fprintf(stdout, "createSound: Able to create sound %s\n", pFile);
		fprintf(stdout, "createSound: pSound=%d\n", pSound);
	}
}

void SoundSystem::createBackgroundMusic(Sound ** pSound, const char* pFile)
{
	FMOD_RESULT result;
	struct stat buffer;

	// check if file exists
	if (stat(pFile, &buffer) != 0) {
		fprintf(stdout, "createSound ERROR: FILE DOES NOT EXIST\n");
	}

	fprintf(stdout, "createSound: before system->createSound pSound=%d\n", pSound);
	// using FMOD_CREATESAMPLE since background music is larger file
	result = system->createSound(pFile, FMOD_CREATESAMPLE, 0, pSound);
	if (result != FMOD_OK) {
		if (result == FMOD_ERR_UNINITIALIZED) {
			fprintf(stdout, "createSound ERROR: FMOD_ERR_UNINITIALIZED\n");
		}
		else if (result == FMOD_ERR_UNSUPPORTED) {
			fprintf(stdout, "createSound ERROR: FMOD_ERR_UNSUPPORTED\n");
		}
		else {
			fprintf(stdout, "createSound ERROR %d: COULD NOT CREATE SOUND %s\n", result, pFile);
		}
	}
	else {
		fprintf(stdout, "createSound: Able to create sound %s\n", pFile);
		fprintf(stdout, "createSound: pSound=%d\n", pSound);
	}
}

// this method will play a sound effect, regardless of other sounds
// are currently playing
void SoundSystem::playSoundEffect(Sound * pSound, bool bLoop)
{
	FMOD_RESULT result;

	if (!bLoop) {
		pSound->setMode(FMOD_LOOP_OFF);
	}
	else {
		pSound->setMode(FMOD_LOOP_NORMAL);
		pSound->setLoopCount(-1);
	}

	result = system->playSound(pSound, 0, false, &channel[0]);

	if (result != FMOD_OK) {
		if (result == FMOD_ERR_INVALID_PARAM) {
			fprintf(stdout, "playSound ERROR: pSound=%d\n", pSound);
			fprintf(stdout, "playSound ERROR: FMOD_ERR_INVALID_PARAM\n");
		}
		else {
			fprintf(stdout, "playSound ERROR %d: COULD NOT PLAY SOUND\n", result);
		}
	}
}

void SoundSystem::pauseSoundEffect()
{
	FMOD_RESULT result;
	bool playing = false;
	result = channel[0]->isPlaying(&playing);

	if (playing) {
		result = channel[0]->setPaused(true);

		// TODO: also reset or clear channel
		if (result != FMOD_OK) {
			if (result == FMOD_ERR_INVALID_PARAM) {
				fprintf(stdout, "pauseSoundEffect ERROR: FMOD_ERR_INVALID_PARAM\n");
			}
			else {
				fprintf(stdout, "pauseSoundEffect ERROR %d: COULD NOT PAUSE SOUND EFFECT\n", result);
			}
		}
	}
}

void SoundSystem::pauseAllSounds()
{
	FMOD_RESULT result;
	bool playing = false;
	result = channel[0]->isPlaying(&playing);

	if (playing) {
		result = channel[0]->setPaused(true);

		// TODO: also reset or clear channel
		if (result != FMOD_OK) {
			if (result == FMOD_ERR_INVALID_PARAM) {
				fprintf(stdout, "pauseAllSounds ERROR: FMOD_ERR_INVALID_PARAM\n");
			}
			else {
				fprintf(stdout, "pauseAllSounds ERROR %d: COULD NOT PAUSE SOUND EFFECT\n", result);
			}
		}
	}
	result = channel[1]->isPlaying(&playing);
	if (playing) {
		result = channel[1]->setPaused(true);

		// TODO: also reset or clear channel
		if (result != FMOD_OK) {
			if (result == FMOD_ERR_INVALID_PARAM) {
				fprintf(stdout, "pauseAllSounds ERROR: FMOD_ERR_INVALID_PARAM\n");
			}
			else {
				fprintf(stdout, "pauseAllSounds ERROR %d: COULD NOT PAUSE SOUND EFFECT\n", result);
			}
		}
	}
	result = channel[2]->isPlaying(&playing);
	if (playing) {
		result = channel[2]->setPaused(true);

		// TODO: also reset or clear channel
		if (result != FMOD_OK) {
			if (result == FMOD_ERR_INVALID_PARAM) {
				fprintf(stdout, "pauseAllSounds ERROR: FMOD_ERR_INVALID_PARAM\n");
			}
			else {
				fprintf(stdout, "pauseAllSounds ERROR %d: COULD NOT PAUSE SOUND EFFECT\n", result);
			}
		}
	}


}

void SoundSystem::playBackgroundMusic(Sound * pSound, bool bLoop)
{
	FMOD_RESULT result;

	if (!bLoop) {
		pSound->setMode(FMOD_LOOP_OFF);
	}
	else {
		pSound->setMode(FMOD_LOOP_NORMAL);
		pSound->setLoopCount(-1);
	}

	result = system->playSound(pSound, 0, false, &channel[2]);
	channel[2]->setVolume(0.05f);

	if (result != FMOD_OK) {
		if (result == FMOD_ERR_INVALID_PARAM) {
			fprintf(stdout, "playSound ERROR: pSound=%d\n", pSound);
			fprintf(stdout, "playSound ERROR: FMOD_ERR_INVALID_PARAM\n");
		}
		else {
			fprintf(stdout, "playSound ERROR %d: COULD NOT PLAY SOUND\n", result);
		}
	}
}

void SoundSystem::playOtherPlayersSounds(Sound * pSound, bool bLoop)
{
	FMOD_RESULT result;

	if (!bLoop) {
		pSound->setMode(FMOD_LOOP_OFF);
	}
	else {
		pSound->setMode(FMOD_LOOP_NORMAL);
		pSound->setLoopCount(-1);
	}

	result = system->playSound(pSound, 0, false, &channel[1]);

	if (result != FMOD_OK) {
		if (result == FMOD_ERR_INVALID_PARAM) {
			fprintf(stdout, "playSound ERROR: pSound=%d\n", pSound);
			fprintf(stdout, "playSound ERROR: FMOD_ERR_INVALID_PARAM\n");
		}
		else {
			fprintf(stdout, "playSound ERROR %d: COULD NOT PLAY SOUND\n", result);
		}
	}
}

// if there are sound effects being played, don't play this
void SoundSystem::playSoundEffectNoOverlap(Sound * pSound, bool bLoop)
{
	FMOD_RESULT result;
	bool playing = false;
	fprintf(stdout, "called playSoundNoOverlap\n");


	result = channel[0]->isPlaying(&playing);
	if (!playing) {
		if (!bLoop) {
			pSound->setMode(FMOD_LOOP_OFF);
		}
		else {
			pSound->setMode(FMOD_LOOP_NORMAL);
			pSound->setLoopCount(-1);
		}

		fprintf(stdout, "playSoundNoOverlap playing sound\n");
		result = system->playSound(pSound, 0, false, &channel[0]);

		if (result != FMOD_OK) {
			if (result == FMOD_ERR_INVALID_PARAM) {
				fprintf(stdout, "playSound ERROR: pSound=%d\n", pSound);
				fprintf(stdout, "playSound ERROR: FMOD_ERR_INVALID_PARAM\n");
			}
			else {
				fprintf(stdout, "playSound ERROR %d: COULD NOT PLAY SOUND\n", result);
			}
		}
	}
	
}

void SoundSystem::releaseSound(Sound * pSound)
{
	pSound->release();
}

bool SoundSystem::shouldIgnoreSound()
{
	return !hasAudioDriver;
}