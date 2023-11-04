#pragma once

#include <filesystem>
#include <string>
#include <glm/glm.hpp>

namespace Voxymore::Audio
{
	using Vector3 = glm::vec3;

    //TODO: Find a way to save and load all the sound definitions from disk
    struct SoundDefinition
    {
        std::string name;
        float defaultVolumedB = 30.0f;
        float minDistance = 0.0f;
        float maxDistance = 100.0f;
        bool is3D = true;
        bool isLooping = false;
        bool isStream = false;
    };

	class Voxaudio
	{
	public:
		static void Init(const std::filesystem::path& configFile = "");
		static void Update(float deltaTimeSeconds);
		static void Shutdown();

        int RegisterSound(const SoundDefinition& soundDef, bool load = true);
        void UnregisterSound(int soundId);

        void LoadSound(int soundId);
        void UnloadSound(int soundId);

        //TODO: bool ShouldBeVirtual(bool allowOneShotVirtuals) const

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