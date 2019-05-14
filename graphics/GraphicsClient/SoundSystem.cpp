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
