//
// Created by ianpo on 01/11/2023.
//

#pragma once

#include <filesystem>
#include <string>
#include <glm/glm.hpp>

namespace Voxymore::Audio
{
	using Vector3 = glm::vec3;
    typedef uint32_t TypeId;

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

    struct OneShotSound
    {
        TypeId SoundId;
        TypeId ChannelId;
    };

	class Voxaudio
	{
	public:
		static void Init(const std::filesystem::path& configFile = "");
		static void Update(float deltaTimeSeconds);
		static void Shutdown();

        static TypeId RegisterSound(const SoundDefinition& soundDef, bool load = true);
        static void UnregisterSound(TypeId soundId);

        static void LoadSound(TypeId soundId);
        static void UnloadSound(TypeId soundId);

        //TODO: bool ShouldBeVirtual(bool allowOneShotVirtuals) const

		static void Set3dListenerAndOrientation(const Vector3& position, const Vector3& look, const Vector3& up);
		static void Set3dListenerAndOrientation(const Vector3& position, const Vector3& look, const Vector3& up, const Vector3& velocity);

		static TypeId PlaySound(TypeId soundId, const Vector3& pos = { 0,0,0 }, float volumedB = 0.0f);
        static OneShotSound PlayOnShot(const SoundDefinition& soundDef, const Vector3& pos = { 0,0,0 }, float volumedB = 0.0f);

		static void StopChannel(TypeId channelId, float fadeTimeSeconds = 0.0f);
		static void StopAllChannels();

		static void SetChannel3dPosition(TypeId channelId, const Vector3& position);
		static void SetChannelVolume(TypeId channelId, float volumedB);
		static bool IsPlaying(TypeId channelId);
	};

    namespace Helper
    {
        float dBToVolume(float dB);
        float VolumeTodB(float Volume);
    }
}