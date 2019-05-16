#pragma once
#include "fmod.hpp"
#include "fmod.h"
//#include <windows.h>

typedef FMOD::Sound* Sound;

class SoundSystem
{
public:
	FMOD::System *system;
	SoundSystem();
	~SoundSystem();

	void createSound(Sound *pSound, const char* pFile);
	void playSound(Sound pSound, bool bLoop = false);
	void releaseSound(Sound pSound);
};

