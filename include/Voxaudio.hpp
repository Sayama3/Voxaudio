#pragma once

namespace Voxymore::Audio
{
	class Engine;

	class Voxaudio
	{
	private:
		Engine* m_Engine;
	public:
		Voxaudio();
		~Voxaudio();
	};
}