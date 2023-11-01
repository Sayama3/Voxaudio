#include "Voxymore/Audio/Sound.hpp"
#include "FmodCoreSound.hpp"

namespace Voxymore::Audio
{

	InternalSound::InternalSound(FMOD::Sound* fmodSound) : m_FmodSound(fmodSound)
	{
		
	}

	InternalSound::~InternalSound()
	{
		m_FmodSound->release();
	}

}
