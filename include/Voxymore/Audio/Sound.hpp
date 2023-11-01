#pragma once

#include <filesystem>

namespace Voxymore::Audio
{
	class InternalSound;

	class Sound
	{
	private:
		std::filesystem::path m_SoundPath;
		InternalSound* m_InternalSound;
	public:
		Sound(const std::filesystem::path&);
		~Sound();

		void Play();
	};
}