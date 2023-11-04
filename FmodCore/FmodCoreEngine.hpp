#pragma once

#include "Voxaudio.hpp"
#include <cmath>
#include <iostream>
#include <map>
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

		void Update();

		int NextChannelId;

        //TODO: Benchmark to see if std::unordered_map is not faster for the usage.
		typedef std::map<std::string, FMOD::Sound*> SoundMap;
		typedef std::map<int, FMOD::Channel*> ChannelMap;

		SoundMap Sounds;
		ChannelMap Channels;
    private:
        void ReadConfigFile();
        void WriteConfigFile();
	};

    namespace FmodHelper
    {
        FMOD_VECTOR VectorToFmod(const Vector3& v);
        std::string GetFmodError(FMOD_RESULT result);
    }
}