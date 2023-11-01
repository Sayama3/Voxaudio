#include "Voxaudio.hpp"
#include "FmodStudioEngine.hpp"
#include <fmod_studio.hpp>

namespace Voxymore::Audio
{
	Voxaudio::Voxaudio() : m_Engine(new Engine())
	{
	}

	Voxaudio::~Voxaudio()
	{
		delete m_Engine;
	}
}