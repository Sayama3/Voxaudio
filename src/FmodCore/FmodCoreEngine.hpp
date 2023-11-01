#pragma once

#include <fmod.hpp>

namespace Voxymore::Audio
{
	class Engine
	{
	private:
		FMOD::System* m_System;
	public:
		Engine();
		~Engine();
	};
}