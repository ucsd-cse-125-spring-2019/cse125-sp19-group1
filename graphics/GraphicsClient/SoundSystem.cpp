#include "SoundSystem.h"
#include <iostream>
#include <sys/stat.h>

#define NULL 0
#define PRIORITY_MAX 0
#define PRIORITY_HIGH 32
#define PRIORITY_DEFAULT 128
#define PRIORITY_LOWEST 256

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
	threeDeeChannel[3] = 0;
	threeDeeSeparate = 0;

	curLoop = 0;

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
	result = system->set3DSettings(1.0, DISTANCE_FACTOR, 1.0);
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

void SoundSystem::createBackgroundMusic(const char* pFile)
{
	FMOD_RESULT result;
	struct stat buffer;

	// check if file exists
	if (stat(pFile, &buffer) != 0) {
		fprintf(stderr, "createBackgroundMusic ERROR: FILE DOES NOT EXIST\n");
	}

	// using FMOD_CREATESAMPLE since background music is larger file
	// result = system->createSound(pFile, FMOD_CREATESAMPLE, 0, pSound);
	result = system->createSound(pFile, FMOD_CREATESAMPLE, 0, &backgroundSounds[curLoop]);
	curLoop++;

	if (result != FMOD_OK) {
		std::cerr << "createBackground Music ERROR: could not create sound - " << FMOD_ErrorString(result) << "\n";
	}
	else {
		fprintf(stderr, "createSound: Able to create sound %s\n", pFile);
	}
}

void SoundSystem::startBackgroundMusic()
{
	FMOD_RESULT result;

	curLoop = 0;
	backgroundSounds[curLoop]->setMode(FMOD_LOOP_NORMAL);
	backgroundSounds[curLoop]->setLoopCount(-1);

	result = system->playSound(backgroundSounds[curLoop], 0, false, &not3DChannel[1]);
	if (result != FMOD_OK) {
		std::cerr << "startBackgroundMusic ERROR: cannot play music - " << FMOD_ErrorString(result) << "\n";
	}
}

void SoundSystem::nextBackgroundLoop(bool bLoop)
{
	FMOD_RESULT result;
	unsigned int sLength;
	unsigned int cPosition;

	result = backgroundSounds[curLoop]->getLength(&sLength, FMOD_TIMEUNIT_MS);
	if (result != FMOD_OK) {
		std::cerr << "nextBackgroundLoop ERROR: cannot get length - " << FMOD_ErrorString(result) << "\n";
	}
	result = not3DChannel[1]->getPosition(&cPosition, FMOD_TIMEUNIT_MS);
	if (result != FMOD_OK) {
		std::cerr << "nextBackgroundLoop ERROR: cannot get position - " << FMOD_ErrorString(result) << "\n";
	}

	while (cPosition < sLength) {
		result = not3DChannel[1]->getPosition(&cPosition, FMOD_TIMEUNIT_MS);
		if (result != FMOD_OK) {
			std::cerr << "nextBackgroundLoop ERROR: cannot get position - " << FMOD_ErrorString(result) << "\n";
		}
	}
	
	result = not3DChannel[1]->stop();
	if (result != FMOD_OK) {
		std::cerr << "nextBackgroundLoop ERROR: stop - " << FMOD_ErrorString(result) << "\n";
	}
	
	curLoop++;

	result = system->playSound(backgroundSounds[curLoop], 0, false, &not3DChannel[1]);
	if (result != FMOD_OK) {
		std::cerr << "nextBackgroundLoop ERROR: playSound - " << FMOD_ErrorString(result) << "\n";
	}
	
}

// toLoop: the song number to skip to in the sequence
// isEndLoop: if it's the end of the game, don't need to wait for the loop to end
void SoundSystem::skipToLoop(int toLoop, bool isEndLoop)
{

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
	// std::cerr << "pauseSoundEffect called\n";
	result = not3DChannel[0]->isPlaying(&playing);
	if (result != FMOD_OK && result != FMOD_ERR_INVALID_HANDLE) {
		std::cerr << "pauseSoundEffect ERROR: isPlaying - " << FMOD_ErrorString(result) << "\n";
	}

	if (playing && result != FMOD_ERR_INVALID_HANDLE) {
		result = not3DChannel[0]->setPaused(true);

		// TODO: also reset or clear channel
		if (result != FMOD_OK) {
			std::cerr << "pauseSoundEffect ERROR: cannot pause sound - " << FMOD_ErrorString(result) << "\n";
		}
		// the below is good, pausing is good here
		// else {
		//	std::cerr << "pauseSoundEffect: paused\n";
		//}
	}
}

void SoundSystem::pauseBackgroundMusic()
{
	FMOD_RESULT result;
	bool playing = false;
	result = not3DChannel[1]->isPlaying(&playing);
	if (result != FMOD_OK && result != FMOD_ERR_INVALID_HANDLE) {
		std::cerr << "pauseBackgroundMusic ERROR: isPlaying - " << FMOD_ErrorString(result) << "\n";
	}

	if (playing && result != FMOD_ERR_INVALID_HANDLE) {
		result = not3DChannel[1]->setPaused(true);

		// TODO: also reset or clear channel
		if (result != FMOD_OK) {
			std::cerr << "pauseBackgroundMusic ERROR: cannot pause sound - " << FMOD_ErrorString(result) << "\n";
		}
	}
}

void SoundSystem::pauseOtherPlayersSounds(int playerNum)
{
	FMOD_RESULT result;
	bool playing = false;
	// FMOD::Channel * curPlayerChannel;

	std::cerr << "pauseOtherPlayerSounds: called\n";
	// if the specific player doesn't have their own channel yet

	// curPlayerChannel = threeDeeChannel[playerNum - 1];
	// fprintf(stdout, "pauseOtherPlayersSounds threeDeeChannel size=%d\n", sizeof(threeDeeChannel) / sizeof(threeDeeChannel[0]));
	// std::cerr << "pauseOtherPlayersSounds: curPlayerChannel=" << curPlayerChannel << "\n";
	// fprintf(stdout, "pauseOtherPlayersSounds before playerID=%d channelID=%d", playerID, otherPlayerChannels.at(playerID));
	
	// result = curPlayerChannel->isPlaying(&playing);
	result = threeDeeChannel[playerNum - 1]->isPlaying(&playing);

	// if it's finished playing, the channel might have been released internally
	if (result != FMOD_OK && result != FMOD_ERR_INVALID_HANDLE) {
		std::cerr << "pauseOtherPlayersSounds ERROR: isPlaying - " << FMOD_ErrorString(result) << "\n";
	}

	if (playing && result != FMOD_ERR_INVALID_HANDLE) {
		// result = curPlayerChannel->setPaused(true);
		result = threeDeeChannel[playerNum - 1]->setPaused(true);


		if (result != FMOD_OK) {
			std::cerr << "pauseOtherPlayersSounds ERROR: cannot pause other player sounds - " << FMOD_ErrorString(result) << "\n";
		}
		else {
			std::cerr << "pauseOtherPlayersSounds: paused\n";
		}
	}

	/*
	if (curPlayerChannel) {
		result = curPlayerChannel->stop();
		if (result != FMOD_OK) {
			std::cerr << "pauseOtherPlayersSounds ERROR: stop() - " << FMOD_ErrorString(result) << "\n";
		}
		else {
			std::cerr << "pauseOtherPlayerSounds: stoped\n";
		}
	}
	*/

}

void SoundSystem::pauseAllSounds()
{
	FMOD_RESULT result;
	bool playing = false;

	// pausing all the not3DChannels
	for (int i = 0; i < 3; i++) {
		result = not3DChannel[i]->isPlaying(&playing);
		if (result != FMOD_OK && result != FMOD_ERR_INVALID_HANDLE) {
			std::cerr << "pauseAllSounds ERROR: isPlaying - " << FMOD_ErrorString(result) << "\n";
		}

		if (playing && result != FMOD_ERR_INVALID_HANDLE) {
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

	result = system->playSound(pSound, 0, true, &not3DChannel[1]);
	result = not3DChannel[1]->setVolume(0.1f);
	result = not3DChannel[1]->setPriority(PRIORITY_MAX);
	result = not3DChannel[1]->setPaused(false);

	if (result != FMOD_OK) {
		std::cerr << "playBackgroundMusic ERROR: cannot play background music - " << FMOD_ErrorString(result) << "\n";
	}
}

void SoundSystem::playOtherPlayersSounds(Sound * pSound, int playerNum, float x, float y, float z, bool playUntilEnd, bool bLoop)
{
	FMOD_RESULT result;
	// FMOD::Channel * curPlayerChannel;
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

	// curPlayerChannel = threeDeeChannel[playerNum - 1];
	// std::cerr << "playOtherPlayerSounds curPlayerChannel=" << curPlayerChannel << "\n";

	// result = system->playSound(pSound, 0, true, &curPlayerChannel);
	if (playUntilEnd) {
		result = system->playSound(pSound, 0, true, &threeDeeSeparate);

		if (result != FMOD_OK) {
			std::cerr << "playOtherPlayersSounds 1 ERROR: cannot play sounds - " << FMOD_ErrorString(result) << "\n";
		}
		// result = curPlayerChannel->set3DAttributes(&loc, NULL, NULL);
		result = threeDeeSeparate->set3DAttributes(&loc, NULL, NULL);

		if (result != FMOD_OK) {
			std::cerr << "playOtherPlayersSounds 1 ERROR: cannot set 3d - " << FMOD_ErrorString(result) << "\n";
		}
		// result = curPlayerChannel->setPaused(false);
		result = threeDeeSeparate->setPaused(false);

		if (result != FMOD_OK) {
			std::cerr << "playOtherPlayersSounds 1 ERROR: cannot set paused to false - " << FMOD_ErrorString(result) << "\n";
		}

	}
	else {
		result = system->playSound(pSound, 0, true, &threeDeeChannel[playerNum - 1]);

		if (result != FMOD_OK) {
			std::cerr << "playOtherPlayersSounds 1 ERROR: cannot play sounds - " << FMOD_ErrorString(result) << "\n";
		}
		// result = curPlayerChannel->set3DAttributes(&loc, NULL, NULL);
		result = threeDeeChannel[playerNum - 1]->set3DAttributes(&loc, NULL, NULL);

		if (result != FMOD_OK) {
			std::cerr << "playOtherPlayersSounds 1 ERROR: cannot set 3d - " << FMOD_ErrorString(result) << "\n";
		}
		// result = curPlayerChannel->setPaused(false);
		result = threeDeeChannel[playerNum - 1]->setPaused(false);

		if (result != FMOD_OK) {
			std::cerr << "playOtherPlayersSounds 1 ERROR: cannot set paused to false - " << FMOD_ErrorString(result) << "\n";
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
			std::cerr << "playSoundEffectNoOverlap ERROR: cannot play sound - " << FMOD_ErrorString(result) << "\n";
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