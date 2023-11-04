#include "Voxaudio.hpp"
#include "FmodCoreEngine.hpp"
#include <fmod.h>

namespace Voxymore::Audio
{
	FmodCoreEngine* s_Engine = nullptr;

	void Voxaudio::Init()
	{
		s_Engine = new FmodCoreEngine();
	}
	void Voxaudio::Update()
	{
		s_Engine->Update();
	}
	void Voxaudio::Shutdown()
	{
		delete s_Engine;
	}

	
	void Voxaudio::LoadSound(const std::string& name, bool is3D = true, bool isLooping = false, bool isStream = false)
	{
		auto soundIt = s_Engine->Sounds.find(name);
		if (soundIt != s_Engine->Sounds.end())
		{
			// The sound is already loaded.
			return;
		}

		FMOD_MODE mode = FMOD_DEFAULT;
		mode |= is3D ? FMOD_3D : FMOD_2D;
		mode |= isLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
		mode |= isStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;
		
		FMOD::Sound* soundPtr = nullptr;
		s_Engine->m_System->createSound(name.c_str(), eMode, nullptr, &soundPtr);
		if (soundPtr)
		{
			s_Engine[name] = soundPtr;
		}
	}
	
	void Voxaudio::UnloadSound(const std::string& name)
	{
		auto soundIt = s_Engine->Sounds.find(name);
		if (soundIt == s_Engine->Sounds.end()) return;
		soundIt->second->release();
		s_Engine->Sounds.erase(soundIt);
	}
	
	void Voxaudio::Set3dListenerAndOrientation(const Vector3& position, const Vector3& loop, const Vector3& up)
	{
		
	}

	int Voxaudio::PlaySound(const std::string& name, const Vector3& pos = { 0,0,0 }, float volumedB = 0.0f)
	{
		int channelId = s_Engine->nextChannelId++;

		auto soundIt = s_Engine->Sounds.find(name);
		if (soundIt == s_Engine->Sounds.end())
		{
			LoadSound(name);
			soundIt = s_Engine->Sounds.find(name);
			if (soundIt == s_Engine->Sounds.end())
			{
				return channelId;
			}
		}

		FMOD::Channel* channel = nullptr;
		s_Engine->m_System->playSound(FMOD_CHANNEL_FREE, soundIt->second, true, &channel);
		if (channel)
		{
			auto position = VectorToFmod(pos);
			channel->set3DAttributes(&position, nullptr);
			channel->setVolume(dbToVolume(volumeDb));
			channel->setPaused(false);
			s_Engine->Channels[channelId] = channel;
		}

		return channelId;
	}
	
	void Voxaudio::StopChannel(int channelId)
	{
		//TODO:
	}
	
	void Voxaudio::StopAllChannels()
	{
		//TODO:
	}

	
	void Voxaudio::SetChannel3dPosition(int channelId, const Vector3& position)
	{
		//TODO: Implement
	}
	
	void Voxaudio::SetChannelVolume(int channelId, float volumedB)
	{
		//TODO: Implement
	}
	
	bool Voxaudio::IsPlaying(int channelId) const
	{
		//TODO: Implement
	}
}