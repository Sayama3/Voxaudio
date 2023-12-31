//
// Created by ianpo on 04/11/2023.
//

#pragma once

#include "Voxaudio.hpp"
#include <cmath>
#include <iostream>
#include <unordered_map>
#include <filesystem>
#include <string>
#include <fmod.hpp>

#if (_MSC_VER && !__INTEL_COMPILER) || (__MINGW32__ || __MINGW64__)
    #define VXM_BREAK __debugbreak()
#elif _POSIX
    #define VXM_BREAK std::raise(SIGTRAP)
#else
    #define VXM_BREAK
#endif

#define Concat(a,b) a##b
#define Combine(a,b) Concat(a,b)
#define FmodResult Combine(result , __LINE__)
#define CheckFmod(func) {FMOD_RESULT FmodResult = func; if(FmodResult != FMOD_RESULT::FMOD_OK) {std::cerr << "FMOD ERROR : " << ::Voxymore::Audio::FmodHelper::GetFmodError(FmodResult) << std::endl; VXM_BREAK;} }

namespace Voxymore::Audio
{
    struct EngineConfig
    {
        int numberOfChannels = 128;
    };

    class AudioFader
    {
    private:
        float FromVolumedB = 0.0f;
        float ToVolumedB = 0.0f;
        float TimeFade = 0.0f;
        float CurrentTime = 0.0f;
    public:
        // Fade in
        void StartFade(float toVolumedB, float fadeTimeSeconds);
        // Fade out
        void StartFade(float fromVolumedB, float toVolumedB, float fadeTimeSeconds);
        // Update fade
        void Update(float deltaTimeSeconds);
        bool IsFinished() const;
        float GetCurrentVolumedB() const;
        float GetCurrentVolume() const;
    };

    struct Sound
    {
        Sound(const SoundDefinition&, bool oneShot = false);

        FMOD::Sound* m_Sound = nullptr;
        SoundDefinition m_Definition;
        bool m_OneShot = false;
    };

    class FmodCoreEngine;

    struct Channel
    {
        Channel(FmodCoreEngine& engine, TypeId soundId, const SoundDefinition& definition, const Vector3& position, float volumedB);
        ~Channel();

        enum class State
        {Initialize, ToPlay, Loading, Playing, Stopping, Stopped, Virtualizing, Virtual, Devirtualize};

        FmodCoreEngine& m_Engine;
        FMOD::Channel* m_Channel;
        TypeId m_SoundId;
        Vector3 m_Position;
        float m_VolumedB = 0.0f;
        float m_SoundVolume = 0.0f;
        State m_State = State::Initialize;
        bool m_StopRequested = false;

        AudioFader m_StopFader;
        AudioFader m_VirtualizeFader;

        void Update(float deltaTime);
        void UpdateChannelParameters();
        bool ShouldBeVirtual(bool allowVirtualOneShot) const;
        bool IsPlaying() const;
        float GetVolumedB() const;
    private:
        bool IsOneShot() const;
        const SoundDefinition* GetSoundDefinition() const;
    };

	class FmodCoreEngine
	{
    private:
        std::filesystem::path ConfigPath;
        EngineConfig Config;
	public:
		FMOD::System* System;
	public:
		FmodCoreEngine(const std::filesystem::path& configPath);
		~FmodCoreEngine();

		void Update(float deltaTimeSecond);

        void LoadSound(TypeId soundId);
        void UnloadSound(TypeId soundId);

        bool SoundIsLoaded(TypeId soundId) const;
    public:
        TypeId NextChannelId;
        TypeId NextSoundId;

        //TODO: Benchmark to see if std::unordered_map is not faster for the usage.
        typedef std::unordered_map<TypeId, std::unique_ptr<Sound>> SoundMap;
        typedef std::unordered_map<TypeId, std::unique_ptr<Channel>> ChannelMap;

        SoundMap Sounds;
        ChannelMap Channels;
    private:
        void ReadConfigFile();
        void WriteConfigFile();
	};

    namespace FmodHelper
    {
        FMOD_VECTOR VectorToFmod(const Vector3& v);
        Vector3 FmodToVector(const FMOD_VECTOR& fv);
        std::string GetFmodError(FMOD_RESULT result);
    }
}