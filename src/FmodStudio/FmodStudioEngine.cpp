#include "FmodStudioEngine.hpp"

namespace Voxymore::Audio
{
	Engine::Engine()
	{
		FMOD::Studio::System::create(&m_System);
		m_System->initialize(128, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr);
		//TODO: Load Bank file from config file.
	}

	Engine::~Engine()
	{

	}
}