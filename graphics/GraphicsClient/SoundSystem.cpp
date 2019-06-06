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
	// threeDeeChannelTaken = 0;
	// threeDeeChannelPlayedBefore[0] = false;
	// threeDeeChannelPlayedBefore[1] = false;
	// threeDeeChannelPlayedBefore[2] = false;

	if (driverCount == 0) {
		std::cerr << "SoundSystem ERROR: driverCount = 0, possibly because no audio devices are plugged in\n";
		hasAudioDriver = false;
		return;
	}
	else {
		hasAudioDriver = true;
		std::cerr << "SoundSystem: Number of Drivers=" << driverCount << "\n";
	}

	result = system->setDriver(NULL);
	if (result != FMOD_OK) {
		std::cerr << "SoundSystem ERROR: setDriver - " << FMOD_ErrorString(result) << "\n";
	}

	result = system->init(32, FMOD_INIT_NORMAL, NULL);

	if (result != FMOD_OK) {
		std::cerr << "SoundSystem ERROR %d: CANNOT INITIALIZE SOUNDSYSTEM - " << FMOD_ErrorString(result) << "\n";
	}

	FMOD_VECTOR forwardOrientation;
	forwardOrientation.x = 0.0;
	forwardOrientation.y = 0.0;
	forwardOrientation.z = -1.0;

	result = system->set3DListenerAttributes(NULL, NULL, NULL, &forwardOrientation, NULL);
	if (result != FMOD_OK) {
		std::cerr << "SoundSystem ERROR %d: CANNOT SET PLAYER POSITION - " << FMOD_ErrorString(result) << "\n";
	}

	// 5.0 is kind of like for distance -- very arbitrary and needs playing with
	result = system->set3DSettings(1.0, 3.0, 1.0);
}


SoundSystem::~SoundSystem()
{
	system->release();
}

void SoundSystem::setListenerLocation(float x, float y, float z)
{
	FMOD_RESULT result;
	FMOD_VECTOR forwardOrientation;
	FMOD_VECTOR loc;

	forwardOrientation.x = 0.0;
	forwardOrientation.y = 0.0;
	forwardOrientation.z = -1.0;

	loc.x = x;
	loc.y = y;
	loc.z = z;

	result = system->set3DListenerAttributes(NULL, &loc, NULL, &forwardOrientation, NULL);
	if (result != FMOD_OK) {
		std::cerr << "setListenerLocation ERROR: cannot set player position - " << FMOD_ErrorString(result) << "\n";
	}
}

void SoundSystem::update()
{
	FMOD_RESULT result;

	result = system->update();
	if (result != FMOD_OK) {
		std::cerr << "update ERROR: cannot update 3D sound calcs - " << FMOD_ErrorString(result) << "\n";
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
		std::cerr << "createSoundEffect ERROR: could not create sound - " << FMOD_ErrorString(result) << "\n";
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
		std::cerr << "createOtherPlayerSounds ERROR: could not create sound - " << FMOD_ErrorString(result) << "\n";
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
		std::cerr << "createBackground Music ERROR: could not create sound - " << FMOD_ErrorString(result) << "\n";
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
		std::cerr << "playSoundEffect ERROR: cannot play sound - " << FMOD_ErrorString(result) << "\n";
	}
}

void SoundSystem::pauseSoundEffect()
{
	FMOD_RESULT result;
	bool playing = false;
	result = not3DChannel[0]->isPlaying(&playing);
	if (result != FMOD_OK) {
		std::cerr << "pauseSoundEffect ERROR: isPlaying - " << FMOD_ErrorString(result) << "\n";
	}

	if (playing) {
		result = not3DChannel[0]->setPaused(true);

		// TODO: also reset or clear channel
		if (result != FMOD_OK) {
			std::cerr << "pauseSoundEffect ERROR: cannot pause sound - " << FMOD_ErrorString(result) << "\n";
		}
	}
}

void SoundSystem::pauseOtherPlayersSounds(int playerNum)
{
	FMOD_RESULT result;
	bool playing = false;
	FMOD::Channel * curPlayerChannel;

	// if the specific player doesn't have their own channel yet

	curPlayerChannel = threeDeeChannel[playerNum - 1];
	// fprintf(stdout, "pauseOtherPlayersSounds threeDeeChannel size=%d\n", sizeof(threeDeeChannel) / sizeof(threeDeeChannel[0]));
	// fprintf(stdout, "pauseOtherPlayersSounds curPlayerChannel=%d\n", curPlayerChannel);
	// fprintf(stdout, "pauseOtherPlayersSounds before playerID=%d channelID=%d", playerID, otherPlayerChannels.at(playerID));
	
	result = curPlayerChannel->isPlaying(&playing);
	if (result != FMOD_OK) {
		std::cerr << "pauseOtherPlayersSounds ERROR: isPlaying - " << FMOD_ErrorString(result) << "\n";
	}

	if (playing) {
		result = curPlayerChannel->setPaused(true);

		if (result != FMOD_OK) {
			std::cerr << "pauseOtherPlayersSounds ERROR: cannot pause other player sounds - " << FMOD_ErrorString(result) << "\n";
		}
	}

}

void SoundSystem::pauseAllSounds()
{
	FMOD_RESULT result;
	bool playing = false;

	// pausing all the not3DChannels
	for (int i = 0; i < 3; i++) {
		result = not3DChannel[i]->isPlaying(&playing);

		if (playing) {
			result = not3DChannel[i]->setPaused(true);

			// TODO: also reset or clear channel
			if (result != FMOD_OK) {
				std::cerr << "pauseAllSounds ERROR: cannot pause not3DChannel[" << i << "0] - " << FMOD_ErrorString(result) << "\n";
			}
		}

	}
	
	// pausing all the 3DChannels
	for (int i = 0; i < 4; i++) {
		result = threeDeeChannel[i]->isPlaying(&playing);
		if (playing) {
			result = threeDeeChannel[i]->setPaused(true);

			// TODO: also reset or clear channel
			if (result != FMOD_OK) {
				std::cerr << "pauseAllSounds ERROR: cannot pause threeDeeChannel[" << i << "0] - " << FMOD_ErrorString(result) << "\n";
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

	result = system->playSound(pSound, 0, false, &not3DChannel[1]);
	not3DChannel[1]->setVolume(0.2f);

	if (result != FMOD_OK) {
		std::cerr << "playBackgroundMusic ERROR: cannot play background music - " << FMOD_ErrorString(result) << "\n";
	}
}

void SoundSystem::playOtherPlayersSounds(Sound * pSound, int playerNum, float x, float y, float z, bool bLoop)
{
	FMOD_RESULT result;
	FMOD::Channel * curPlayerChannel;
	bool paused = true;
	bool playing = false;
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

	std::cerr << "playOtherPlayerSounds: playerNum=" << playerNum << "\n";

	curPlayerChannel = threeDeeChannel[playerNum - 1];
	std::cerr << "playOtherPlayerSounds curPlayerChannel=" << curPlayerChannel << "\n";

	if (threeDeeChannelPlayedBefore[playerNum - 1] == false) {
		result = system->playSound(pSound, 0, true, &curPlayerChannel);
		if (result != FMOD_OK) {
			std::cerr << "playOtherPlayersSounds 1 ERROR: cannot play sounds - " << FMOD_ErrorString(result) << "\n";
		}
		result = curPlayerChannel->set3DAttributes(&loc, NULL, NULL);
		if (result != FMOD_OK) {
			std::cerr << "playOtherPlayersSounds 1 ERROR: cannot set 3d - " << FMOD_ErrorString(result) << "\n";
		}
		result = curPlayerChannel->setPaused(false);
		if (result != FMOD_OK) {
			std::cerr << "playOtherPlayersSounds 1 ERROR: cannot set paused to false - " << FMOD_ErrorString(result) << "\n";
		}

		threeDeeChannelPlayedBefore[playerNum - 1] = true;
	}
	else {
		result = curPlayerChannel->getPaused(&paused);
		if (result != FMOD_OK) {
			std::cerr << "playOtherPlayersSounds ERROR: getPaused - " << FMOD_ErrorString(result) << "\n";
		}
		result = curPlayerChannel->isPlaying(&playing);
		if (result != FMOD_OK) {
			std::cerr << "playOtherPlayersSounds ERROR: isPlaying" << FMOD_ErrorString(result) << "\n";
		}

		std::cerr << "paused=" << paused << " playing=" << playing << "\n";

		if (paused) { // potential FIXME: need playing too?
			std::cerr << "playOtherPlayersSounds playerNum=" << playerNum << "playing=" 
				<< playing << "paused=" << paused << "\n";

			result = system->playSound(pSound, 0, true, &curPlayerChannel);
			if (result != FMOD_OK) {
				std::cerr << "playOtherPlayersSounds 2 ERROR: cannot play sounds - " << FMOD_ErrorString(result) << "\n";
			}
			result = curPlayerChannel->set3DAttributes(&loc, NULL, NULL);
			if (result != FMOD_OK) {
				std::cerr << "playOtherPlayersSounds 2 ERROR: cannot set 3d - " << FMOD_ErrorString(result) << "\n";
			}
			result = curPlayerChannel->setPaused(false);
			if (result != FMOD_OK) {
				std::cerr << "playOtherPlayersSounds 2 ERROR: cannot set paused to false - " << FMOD_ErrorString(result) << "\n";
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