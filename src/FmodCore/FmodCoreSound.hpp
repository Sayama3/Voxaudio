#pragma once

#include <filesystem>
#include <fmod.hpp>

namespace Voxymore::Audio
{
	class InternalSound
	{
	private:
		FMOD::Sound* m_FmodSound;
	public:
		InternalSound(FMOD::Sound* fmodSound);
		~InternalSound();
	};
}