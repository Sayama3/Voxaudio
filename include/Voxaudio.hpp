#pragma once

#include <filesystem>
#include <string>
#include <glm/glm.hpp>

namespace Voxymore::Audio
{
	using Vector3 = glm::vec3;

	class Voxaudio
	{
	public:
		static void Init(const std::filesystem::path& configFile = "");
		static void Update();
		static void Shutdown();

		void LoadSound(const std::string& name, bool is3D = true, bool isLooping = false, bool isStream = false);
		void UnloadSound(const std::string& name);

		void Set3dListenerAndOrientation(const Vector3& position, const Vector3& look, const Vector3& up);
		void Set3dListenerAndOrientation(const Vector3& position, const Vector3& look, const Vector3& up, const Vector3& velocity);

		int PlaySound(const std::string& name, const Vector3& pos = { 0,0,0 }, float volumedB = 0.0f);
		void StopChannel(int channelId);
		void StopAllChannels();

		void SetChannel3dPosition(int channelId, const Vector3& position);
		void SetChannelVolume(int channelId, float volumedB);
		bool IsPlaying(int channelId) const;
	};

    namespace Helper
    {
        float dBToVolume(float dB);
        float VolumeTodB(float Volume);
    }
}