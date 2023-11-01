#pragma once

#include <fmod_studio.hpp>

namespace Voxymore::Audio
{
	class Engine
	{
	private:
		FMOD::Studio::System* m_System = nullptr;
		FMOD::Studio::Bank* m_Bank = nullptr;
	public:
		Engine();
		~Engine();
	};
}