#pragma once
#include "fmod.hpp"

// Referenced https://cuboidzone.wordpress.com/2013/07/26/tutorial-implementing-fmod/ 
// in setting up this class

typedef FMOD::Sound* Sound; // SoundClass in tutorial

class SoundSystem
{
public:
	FMOD::System *m_pSystem;
	SoundSystem();
	~SoundSystem();
};

