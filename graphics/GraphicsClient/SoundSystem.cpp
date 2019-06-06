#include "SoundSystem.h"
#include <iostream>
#include <sys/stat.h>

#define NULL 0

SoundSystem::SoundSystem()
{
	FMOD_RESULT result;

	hasAudioDriver = false;
	continueQueue = false;

	if (FMOD::System_Create(&system) != FMOD_OK) {
		// Report Error
		return;
	}

	int driverCount = 0;
	system->getNumDrivers(&driverCount);
	not3DChannel[0] = 0;
	not3DChannel[1] = 0;
	not3DChannel[2] = 0;
	threeDeeChannel[0] = 0;
	threeDeeChannel[1] = 0;
	threeDeeChannel[2] = 0;
	threeDeeChannelTaken = 0;
	threeDeeChannelPlayedBefore[0] = false;
	threeDeeChannelPlayedBefore[1] = false;
	threeDeeChannelPlayedBefore[2] = false;

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

	result = system->set3DListenerAttributes(NULL, NULL, NULL, NULL, NULL);
	if (result != FMOD_OK) {
		fprintf(stdout, "SoundSystem ERROR %d: CANNOT SET PLAYER POSITION\n", result);
	}

	// 5.0 is kind of like for distance -- very arbitrary and needs playing with
	result = system->set3DSettings(1.0, 3.0, 1.0);
}


SoundSystem::~SoundSystem()
{
}

void SoundSystem::setListenerLocation(float x, float y, float z)
{
	FMOD_RESULT result;
	FMOD_VECTOR forwardOrientation;
	FMOD_VECTOR loc;

	forwardOrientation.x = 0.0;
	forwardOrientation.y = 0.0;
	forwardOrientation.z = 1.0;

	loc.x = x;
	loc.y = y;
	loc.z = z;

	result = system->set3DListenerAttributes(NULL, &loc, NULL, NULL, NULL);
	if (result != FMOD_OK) {
		std::cerr << "setListenerLocation ERROR: cannot set player position - ";
		errorCheck(result);
	}
}

void SoundSystem::update()
{
	FMOD_RESULT result;

	result = system->update();
	if (result != FMOD_OK) {
		std::cerr << "update ERROR: cannot update 3D sound calcs - ";
		errorCheck(result);
	}

}

void SoundSystem::createSoundEffect(Sound ** pSound, const char* pFile)
{
	FMOD_RESULT result;
	struct stat buffer;

	// check if file exists
	if (stat(pFile, &buffer) != 0) {
		std::cerr << "createSound ERROR: FILE DOES NOT EXIST\n";
	}

	fprintf(stderr, "createSound: before system->createSound pSound=%d\n", pSound);
	// can also use FMOD_CREATESAMPLE to load entire sound
	// and decompress it in memory to speed up playback
	result = system->createSound(pFile, FMOD_DEFAULT, 0, pSound);
	if (result != FMOD_OK) {
		std::cerr << "createSoundEffect ERROR: could not create sound - ";
		errorCheck(result);
	}
	// else {
	//	fprintf(stderr, "createSound: Able to create sound %s\n", pFile);
	//	fprintf(stderr, "createSound: pSound=%d\n", pSound);
	// }
}

void SoundSystem::createOtherPlayersSounds(Sound ** pSound, const char* pFile)
{
	FMOD_RESULT result;
	struct stat buffer;

	// check if file exists
	if (stat(pFile, &buffer) != 0) {
		fprintf(stderr, "createOtherPlayerSounds ERROR: FILE DOES NOT EXIST\n");
	}

	fprintf(stderr, "createOtherPlayerSounds: before system->createSound pSound=%d\n", pSound);
	// can also use FMOD_CREATESAMPLE to load entire sound
	// and decompress it in memory to speed up playback
	result = system->createSound(pFile, FMOD_3D, 0, pSound);
	if (result != FMOD_OK) {
		std::cerr << "createOtherPlayerSounds ERROR: could not create sound - ";
		errorCheck(result);
	}
	else {
		fprintf(stderr, "createOtherPlayersSounds: Able to create sound %s\n", pFile);
		fprintf(stderr, "createOtherPlayersSounds: pSound=%d\n", pSound);
	}
}

void SoundSystem::createBackgroundMusic(Sound ** pSound, const char* pFile)
{
	FMOD_RESULT result;
	struct stat buffer;

	// check if file exists
	if (stat(pFile, &buffer) != 0) {
		fprintf(stderr, "createBackgroundMusic ERROR: FILE DOES NOT EXIST\n");
	}

	fprintf(stderr, "createBackgroundMusic: before system->createSound pSound=%d\n", pSound);
	// using FMOD_CREATESAMPLE since background music is larger file
	result = system->createSound(pFile, FMOD_CREATESAMPLE, 0, pSound);
	if (result != FMOD_OK) {
		std::cerr << "createBackground Music ERROR: could not create sound - ";
		errorCheck(result);
	}
	else {
		fprintf(stderr, "createSound: Able to create sound %s\n", pFile);
		fprintf(stderr, "createSound: pSound=%d\n", pSound);
	}
}

// this method will play a sound effect, regardless of other sounds
// are currently playing
void SoundSystem::playSoundEffect(Sound * pSound, bool playUntilEnd, bool bLoop)
{
	FMOD_RESULT result;

	if (!bLoop) {
		pSound->setMode(FMOD_LOOP_OFF);
	}
	else {
		pSound->setMode(FMOD_LOOP_NORMAL);
		pSound->setLoopCount(-1);
	}

	if (playUntilEnd) {
		result = system->playSound(pSound, 0, false, &not3DChannel[2]);
	}
	else {
		result = system->playSound(pSound, 0, false, &not3DChannel[0]);
	}
	
	if (result != FMOD_OK) {
		std::cerr << "playSoundEffect ERROR: cannot play sound - ";
		errorCheck(result);
	}
}

void SoundSystem::pauseSoundEffect()
{
	FMOD_RESULT result;
	bool playing = false;
	result = not3DChannel[0]->isPlaying(&playing);

	if (playing) {
		result = not3DChannel[0]->setPaused(true);

		// TODO: also reset or clear channel
		if (result != FMOD_OK) {
			std::cerr << "pauseSoundEffect ERROR: cannot pause sound - ";
			errorCheck(result);
		}
	}
}

void SoundSystem::pauseOtherPlayersSounds(int playerNum)
{
	FMOD_RESULT result;
	FMOD::Channel * curPlayerChannel;

	// if the specific player doesn't have their own channel yet

	curPlayerChannel = threeDeeChannel[playerNum - 1];
	// fprintf(stdout, "pauseOtherPlayersSounds threeDeeChannel size=%d\n", sizeof(threeDeeChannel) / sizeof(threeDeeChannel[0]));
	// fprintf(stdout, "pauseOtherPlayersSounds curPlayerChannel=%d\n", curPlayerChannel);
	// fprintf(stdout, "pauseOtherPlayersSounds before playerID=%d channelID=%d", playerID, otherPlayerChannels.at(playerID));
	result = curPlayerChannel->setPaused(true);
	
	if (result != FMOD_OK) {
		std::cerr << "pauseOtherPlayersSounds ERROR: cannot pause other player sounds - ";
		errorCheck(result);
	}

}

void SoundSystem::pauseAllSounds()
{
	FMOD_RESULT result;
	bool playing = false;
	result = not3DChannel[0]->isPlaying(&playing);

	if (playing) {
		result = not3DChannel[0]->setPaused(true);

		// TODO: also reset or clear channel
		if (result != FMOD_OK) {
			std::cerr << "pauseAllSounds ERROR: cannot pause all sounds - ";
			errorCheck(result);
		}
	}
	result = not3DChannel[1]->isPlaying(&playing);
	if (playing) {
		result = not3DChannel[1]->setPaused(true);

		// TODO: also reset or clear channel
		if (result != FMOD_OK) {
			std::cerr << "pauseAllSounds ERROR: cannot pause all sounds - ";
			errorCheck(result);
		}
	}

	// TODO: pause for all channels of the channelgroup too


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

	result = system->playSound(pSound, 0, false, &not3DChannel[1]);
	not3DChannel[1]->setVolume(0.05f);

	if (result != FMOD_OK) {
		std::cerr << "playBackgroundMusic ERROR: cannot play background music - ";
		errorCheck(result);
	}
}

void SoundSystem::playOtherPlayersSounds(Sound * pSound, int playerNum, float x, float y, float z, bool bLoop)
{
	FMOD_RESULT result;
	FMOD::Channel * curPlayerChannel;
	FMOD_VECTOR loc;
	loc.x = x;
	loc.y = y;
	loc.z = z;

	if (!bLoop) {
		pSound->setMode(FMOD_LOOP_OFF);
	}
	else {
		pSound->setMode(FMOD_LOOP_NORMAL);
		pSound->setLoopCount(-1);
	}

	fprintf(stdout, "playOtherPlayerSounds for playerNum=%d", playerNum);

	curPlayerChannel = threeDeeChannel[playerNum - 1];
	fprintf(stdout, "playOtherPlayerSounds curPlayerChannel=%d\n", curPlayerChannel);

	// haven't played before
	if (threeDeeChannelPlayedBefore[playerNum - 1] == false) {
		result = system->playSound(pSound, 0, true, &curPlayerChannel);
		result = curPlayerChannel->set3DAttributes(&loc, NULL, NULL);
		result = curPlayerChannel->setPaused(false);

		if (result != FMOD_OK) {
			std::cerr << "playOtherPlayersSounds ERROR: cannot play sounds - ";
			errorCheck(result);
		}
		// threeDeeChannelPlayedBefore[otherPlayerChannels.at(playerID)] = true;
	}
	else {
		// just for testing, remove
		bool paused; // = false;
		bool playing; // = false;
		result = curPlayerChannel->getPaused(&paused);
		if (result != FMOD_OK) {
			fprintf(stdout, "playOtherPlayersSounds ERROR %d: getPaused\n");
		}
		result = curPlayerChannel->isPlaying(&playing);
		if (result != FMOD_OK) {
			fprintf(stdout, "playOtherPlayersSounds ERROR %d: isPlaying\n");
		}
		fprintf(stdout, "paused=%d playing=%d\n", paused, playing);
		if (!playing && paused) {
			fprintf(stdout, "playOtherPlayersSounds playerNum=%d playing=%d paused=%d\n", playerNum, playing, paused);
			curPlayerChannel->set3DAttributes(&loc, NULL, NULL);
			result = system->playSound(pSound, 0, false, &curPlayerChannel);

			if (result != FMOD_OK) {
				if (result == FMOD_ERR_INVALID_PARAM) {
					fprintf(stdout, "playOtherPlayersSounds ERROR: pSound=%p\n", pSound);
					fprintf(stdout, "playOtherPlayersSounds ERROR: FMOD_ERR_INVALID_PARAM\n");
				}
				else {
					fprintf(stdout, "playOtherPlayersSounds ERROR %d: COULD NOT PLAY SOUND\n", result);
				}
			}
		}
	}

}

// if there are sound effects being played, don't play this
void SoundSystem::playSoundEffectNoOverlap(Sound * pSound, bool bLoop)
{
	FMOD_RESULT result;
	bool playing = false;
	fprintf(stdout, "called playSoundNoOverlap\n");


	result = not3DChannel[0]->isPlaying(&playing);
	if (!playing) {
		if (!bLoop) {
			pSound->setMode(FMOD_LOOP_OFF);
		}
		else {
			pSound->setMode(FMOD_LOOP_NORMAL);
			pSound->setLoopCount(-1);
		}

		fprintf(stdout, "playSoundNoOverlap playing sound\n");
		result = system->playSound(pSound, 0, false, &not3DChannel[0]);

		if (result != FMOD_OK) {
			std::cerr << "playSoundEffectNoOverlap ERROR: cannot play sound - ";
			errorCheck(result);
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

void SoundSystem::errorCheck(FMOD_RESULT result) {
	if (result != FMOD_OK) {
		if (result == FMOD_ERR_BADCOMMAND) {
			std::cerr << "FMOD_ERR_BADCOMMAND\n";
		}
		else if (result == FMOD_ERR_CHANNEL_STOLEN) {
			std::cerr << "FMOD_ERR_CHANNEL_STOLEN\n";
		}
		else if (result == FMOD_ERR_CHANNEL_ALLOC) {
			std::cerr << "FMOD_ERR_CHANNEL_ALLOC\n";
		}
		else if (result == FMOD_ERR_CHANNEL_STOLEN) {
			std::cerr << "FMOD_ERR_CHANNEL_STOLEN\n";
		}
		else if (result == FMOD_ERR_FILE_BAD) {
			std::cerr << "FMOD_ERR_FILE_BAD\n";
		}
		else if (result == FMOD_ERR_FILE_EOF) {
			std::cerr << "FMOD_ERR_FILE_EOF\n";
		}
		else if (result == FMOD_ERR_FORMAT) {
			std::cerr << "FMOD_ERR_FORMAT\n";
		}
		else if (result == FMOD_ERR_MEMORY) {
			std::cerr << "FMOD_ERR_MEMORY\n";
		}
		else if (result == FMOD_ERR_INITIALIZATION) {
			std::cerr << "FMOD_ERR_INITIALIZATION\n";
		}
		else if (result == FMOD_ERR_INITIALIZED) {
			std::cerr << "FMOD_ERR_INITIALIZED\n";
		}
		else if (result == FMOD_ERR_NEEDS3D) {
			std::cerr << "FMOD_ERR_NEEDS3D\n";
		}
		else if (result == FMOD_ERR_INVALID_HANDLE) {
			std::cerr << "FMOD_ERR_INVALID_HANDLE\n";
		}
		else if (result == FMOD_ERR_INVALID_PARAM) {
			std::cerr << "FMOD_ERR_INVALID_PARAM\n";
		}
		else if (result == FMOD_ERR_INVALID_FLOAT) {
			std::cerr << "FMOD_ERR_INVALID_FLOAT\n";
		}
		else {
			std::cerr << "FMOD Error Number: " << result << "\n";
		}
	}
}