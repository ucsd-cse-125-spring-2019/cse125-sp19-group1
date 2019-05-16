#include "SoundSystem.h"
#define NULL 0

SoundSystem::SoundSystem()
{
	if (FMOD::System_Create(&m_pSystem) != FMOD_OK) {
		// Report Error
		return;
	}

	int driverCount = 0;
	m_pSystem->getNumDrivers(&driverCount);

	if (driverCount == 0) {
		// Report Error
		return;
	}

	// Initialize our Instance with 36 Channels, says tutorial
	m_pSystem->init(36, FMOD_INIT_NORMAL, NULL);
}


SoundSystem::~SoundSystem()
{
}

void SoundSystem::createSound(Sound *pSound, const char* pFile)
{
	// can also use FMOD_CREATESAMPLE to load entire sound
	// and decompress it in memory to speed up playback
	m_pSystem->createSound(pFile, FMOD_DEFAULT, 0, pSound);
}

void SoundSystem::playSound(Sound pSound, bool bLoop)
{
	if (!bLoop) {
		pSound->setMode(FMOD_LOOP_OFF);
	}
	else {
		pSound->setMode(FMOD_LOOP_NORMAL);
		pSound->setLoopCount(-1);
	}

	m_pSystem->playSound(pSound, NULL, false, 0);
}

void SoundSystem::releaseSound(Sound pSound)
{
	pSound->release();
}