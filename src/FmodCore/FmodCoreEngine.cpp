#include "FmodCoreEngine.hpp"

namespace Voxymore::Audio
{

	Engine::Engine()
	{
		FMOD::System_Create(&m_System);
		m_System->init(128, FMOD_INIT_NORMAL, nullptr);
	}
	
	Engine::~Engine()
	{
		m_System->release();
	}
}