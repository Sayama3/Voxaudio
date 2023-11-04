#pragma once

#include <cmath>
#include <map>
#include <string>
#include <fmod.hpp>
#include "Voxaudio.hpp"

namespace Voxymore::Audio
{
	class FmodCoreEngine
	{
	public:
		FMOD::System* m_System;
	public:
		FmodCoreEngine();
		~FmodCoreEngine();

		void Update();

		FMOD::System* System;
		int nextChannelId;

		typedef std::map<std::string, FMOD::Sound*> SoundMap;
		typedef std::map<int, FMOD::Channel*> ChannelMap;

		SoundMap Sounds;
		ChannelMap Channels;
	};

	inline FMOD_VECTOR VectorToFmod(const Vector3& v)
	{
		FMOD_VECTOR fv;
		fv.x = v[0];
		fv.y = v[1];
		fv.z = v[2];
		return fv;
	}
	//TODO: float dbToVolume(float)
	//TODO: float VolumeTodB(float)
}