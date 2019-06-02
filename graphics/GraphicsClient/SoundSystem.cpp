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
	threeDeeChannel[0] = 0;
	threeDeeChannel[1] = 0;
	threeDeeChannel[2] = 0;
	threeDeeChannelTaken = 0;

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
	result = system->set3DSettings(1.0, 5.0, 1.0);
}


SoundSystem::~SoundSystem()
{
}

void SoundSystem::setListenerLocation(float x, float y, float z)
{
	FMOD_RESULT result;
	FMOD_VECTOR loc;
	loc.x = x;
	loc.y = y;
	loc.z = z;

	result = system->set3DListenerAttributes(NULL, &loc, NULL, NULL, NULL);
	if (result != FMOD_OK) {
		fprintf(stdout, "setListenerLocation ERROR %d: CANNOT SET PLAYER POSITION\n", result);
	}
}

void SoundSystem::update()
{
	FMOD_RESULT result;

	result = system->update();
	if (result != FMOD_OK) {
		fprintf(stdout, "update ERROR %d: CANNOT UPDATE 3D SOUND CALCULATIONS\n", result);
	}

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

void SoundSystem::createOtherPlayersSounds(Sound ** pSound, const char* pFile)
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
	result = system->createSound(pFile, FMOD_3D, 0, pSound);
	if (result != FMOD_OK) {
		if (result == FMOD_ERR_UNINITIALIZED) {
			fprintf(stdout, "createOtherPlayersSounds ERROR: FMOD_ERR_UNINITIALIZED\n");
		}
		else if (result == FMOD_ERR_UNSUPPORTED) {
			fprintf(stdout, "createOtherPlayersSounds ERROR: FMOD_ERR_UNSUPPORTED\n");
		}
		else {
			fprintf(stdout, "createOtherPlayersSounds ERROR %d: COULD NOT CREATE SOUND %s\n", result, pFile);
		}
	}
	else {
		fprintf(stdout, "createOtherPlayersSounds: Able to create sound %s\n", pFile);
		fprintf(stdout, "createOtherPlayersSounds: pSound=%d\n", pSound);
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

	result = system->playSound(pSound, 0, false, &not3DChannel[0]);

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
	result = not3DChannel[0]->isPlaying(&playing);

	if (playing) {
		result = not3DChannel[0]->setPaused(true);

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

void SoundSystem::pauseOtherPlayersSounds(int playerID)
{
	FMOD_RESULT result;
	std::map<int, int>::iterator it;
	FMOD::Channel * curPlayerChannel;

	// if the specific player doesn't have their own channel yet
	it = otherPlayerChannels.find(playerID);
	if (it != otherPlayerChannels.end()) {
		if (threeDeeChannelTaken >= sizeof(threeDeeChannel) / sizeof(threeDeeChannel[0])) {
			std::cerr << "WARNING: threeDeeChannelTaken = " << threeDeeChannelTaken << std::endl;
		}
		else {
			std::lock_guard<std::mutex> lock(otherPlayerChannelsM);
			otherPlayerChannels.insert(std::pair<int, int>(playerID, threeDeeChannelTaken));
			threeDeeChannelTaken++;
		}
	
		curPlayerChannel = threeDeeChannel[otherPlayerChannels.at(playerID)];
		std::lock_guard<std::mutex> lock(threeDeeChannelM[otherPlayerChannels.at(playerID)]);
		result = curPlayerChannel->setPaused(true);
	
		if (result != FMOD_OK) {
			if (result == FMOD_ERR_INVALID_PARAM) {
				fprintf(stdout, "pauseOtherPlayersSounds ERROR: FMOD_ERR_INVALID_PARAM\n");
			}
			else {
				fprintf(stdout, "pauseOtherPlayersSounds ERROR %d: COULD NOT PLAY SOUND\n", result);
			}
		}

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
			if (result == FMOD_ERR_INVALID_PARAM) {
				fprintf(stdout, "pauseAllSounds ERROR: FMOD_ERR_INVALID_PARAM\n");
			}
			else {
				fprintf(stdout, "pauseAllSounds ERROR %d: COULD NOT PAUSE SOUND EFFECT\n", result);
			}
		}
	}
	result = not3DChannel[1]->isPlaying(&playing);
	if (playing) {
		result = not3DChannel[1]->setPaused(true);

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
		if (result == FMOD_ERR_INVALID_PARAM) {
			fprintf(stdout, "playSound ERROR: pSound=%p\n", pSound);
			fprintf(stdout, "playSound ERROR: FMOD_ERR_INVALID_PARAM\n");
		}
		else {
			fprintf(stdout, "playSound ERROR %d: COULD NOT PLAY SOUND\n", result);
		}
	}
}

void SoundSystem::playOtherPlayersSounds(Sound * pSound, int playerID, float x, float y, float z, bool bLoop)
{
	FMOD_RESULT result;
	std::map<int, int>::iterator it;
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

	// if the specific player doesn't have their own channel yet
	it = otherPlayerChannels.find(playerID);
	if (it == otherPlayerChannels.end()) {
		if (threeDeeChannelTaken >= sizeof(threeDeeChannel) / sizeof(threeDeeChannel[0])) {
			std::cerr << "WARNING: threeDeeChannelTaken = " << threeDeeChannelTaken << std::endl;
			fprintf(stdout, "playOtherPlayersSounds ERROR: playerID: %d does not have a channel?\n", playerID);
			return;
		}
		else {
			std::lock_guard<std::mutex> lock(otherPlayerChannelsM);
			otherPlayerChannels.insert(std::pair<int, int>(playerID, threeDeeChannelTaken));
			threeDeeChannelTaken++;
		}
	}

	curPlayerChannel = threeDeeChannel[otherPlayerChannels.at(playerID)];

	// just for testing, remove
	bool paused;
	bool playing;
	curPlayerChannel->getPaused(&paused);
	curPlayerChannel->isPlaying(&playing);
	if (!playing && paused) {
		std::lock_guard<std::mutex> lock(threeDeeChannelM[otherPlayerChannels.at(playerID)]);
		curPlayerChannel->set3DAttributes(&loc, NULL, NULL);
		result = system->playSound(pSound, 0, false, &curPlayerChannel);

		if (result != FMOD_OK) {
			if (result == FMOD_ERR_INVALID_PARAM) {
				fprintf(stdout, "playSound ERROR: pSound=%p\n", pSound);
				fprintf(stdout, "playSound ERROR: FMOD_ERR_INVALID_PARAM\n");
			}
			else {
				fprintf(stdout, "playSound ERROR %d: COULD NOT PLAY SOUND\n", result);
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
			if (result == FMOD_ERR_INVALID_PARAM) {
				fprintf(stdout, "playSound ERROR: pSound=%p\n", pSound);
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

void SoundSystem::pushSoundQueue(Sound * pSound)
{
	soundQueue.push(pSound);
}

void SoundSystem::playSoundsInQueueThread()
{
	FMOD_RESULT result;
	bool playing = false;
	bool paused = true;
	Sound * currSound;

	result = not3DChannel[0]->isPlaying(&playing);

	fprintf(stdout, "playSoundsInQueueThread: I'm %d\n", std::this_thread::get_id());
	fprintf(stdout, "playSoundsInQueueThread: num in queue: %llu, playing=%d\n", soundQueue.size(), playing);

	while ((!soundQueue.empty()) && (continueQueue)) {
		result = not3DChannel[0]->isPlaying(&playing);
		result = not3DChannel[0]->getPaused(&paused);
		
		// double check if queue is empty due to multithreading
		if ((!playing || paused) && !soundQueue.empty()) {
			currSound = soundQueue.front();
			soundQueue.pop();

			result = system->playSound(currSound, 0, false, &not3DChannel[0]);

			if (result != FMOD_OK) {
				if (result == FMOD_ERR_INVALID_PARAM) {
					fprintf(stdout, "playSoundsInQueueThread ERROR: pSound=%p\n", currSound);
					fprintf(stdout, "playSoundsInQueueThread ERROR: FMOD_ERR_INVALID_PARAM\n");
				}
				else {
					fprintf(stdout, "playSoundsInQueueThread ERROR %d: COULD NOT PLAY SOUND\n", result);
				}
			}
		}
	}
	fprintf(stdout, "playSoundsInQueueThread: stopping now\n");
}

void SoundSystem::playSoundsInQueue()
{
	if (!continueQueue) {
		fprintf(stdout, "playSoundsInQueue: spawning thread\n");
		continueQueue = true;
		std::thread (&SoundSystem::playSoundsInQueueThread, this).detach();
	}
}

void SoundSystem::pauseSoundQueue()
{
	FMOD_RESULT result;
	bool playing = false;
	bool paused = true;

	result = not3DChannel[0]->isPlaying(&playing);
	result = not3DChannel[0]->getPaused(&paused);

	if (playing || continueQueue || !paused) {
		fprintf(stdout, "pauseSoundQueue: pausing and emptying queue\n");

		std::lock_guard<std::mutex> lock(m);
		continueQueue = false;

		if (playing) {
			result = not3DChannel[0]->setPaused(true);

			while (!soundQueue.empty()) {
				soundQueue.pop();
			}

			// TODO: also reset or clear channel
			if (result != FMOD_OK) {
				if (result == FMOD_ERR_INVALID_PARAM) {
					fprintf(stdout, "pauseSoundQueue ERROR: FMOD_ERR_INVALID_PARAM\n");
				}
				else {
					fprintf(stdout, "pauseSoundQueue ERROR %d: COULD NOT PAUSE SOUND EFFECT\n", result);
				}
			}
		}
	}
}