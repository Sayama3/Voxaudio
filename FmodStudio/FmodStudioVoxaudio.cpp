//
// Created by ianpo on 04/11/2023.
//

#include "Voxaudio.hpp"
#include "FmodStudioEngine.hpp"
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define VIRTUALIZE_FADE_TIME 1.0f
#define SILENCE_dB 0.0f

namespace Voxymore::Audio
{
    FmodStudioEngine* s_Engine = nullptr;

    float Helper::dBToVolume(float dB)
    {
        return std::pow(10.0f, 0.05f * dB);
    }

    float Helper::VolumeTodB(float Volume)
    {
        return 20.0f * std::log10(Volume);
    }

    void Voxaudio::Init(const std::filesystem::path& configFile)
    {
        s_Engine = new FmodStudioEngine(configFile);
    }

    void Voxaudio::Update(float deltaTimeSeconds)
    {
        s_Engine->Update(deltaTimeSeconds);
    }

    void Voxaudio::Shutdown()
    {
        delete s_Engine;
    }

    TypeId Voxaudio::RegisterSound(const SoundDefinition& soundDef, bool load)
    {
        TypeId soundId = s_Engine->NextSoundId++;

        s_Engine->Sounds[soundId] = std::make_unique<Sound>(soundDef);

        if(load)
        {
            LoadSound(soundId);
        }

        return soundId;
    }

    void Voxaudio::UnregisterSound(TypeId soundId)
    {
        auto soundIt = s_Engine->Sounds.find(soundId);
        if (soundIt == s_Engine->Sounds.end()) return;
        s_Engine->Sounds.erase(soundIt);
    }

    void Voxaudio::LoadSound(TypeId soundId)
    {
        s_Engine->LoadSound(soundId);
    }

    void Voxaudio::UnloadSound(TypeId soundId)
    {
        s_Engine->UnloadSound(soundId);
    }

    void Voxaudio::Set3dListenerAndOrientation(const Vector3& position, const Vector3& look, const Vector3& up)
    {
        FMOD_3D_ATTRIBUTES attr;
        attr.position = FmodHelper::VectorToFmod(position);
        attr.forward = FmodHelper::VectorToFmod(look);
        attr.up = FmodHelper::VectorToFmod(up);

        //TODO: Add a multi-listener system.
        CheckFmod(s_Engine->System->setListenerAttributes(0, &attr));
    }

    void Voxaudio::Set3dListenerAndOrientation(const Vector3& position, const Vector3& look, const Vector3& up, const Vector3& velocity)
    {
        FMOD_3D_ATTRIBUTES attr;
        attr.position = FmodHelper::VectorToFmod(position);
        attr.forward = FmodHelper::VectorToFmod(look);
        attr.up = FmodHelper::VectorToFmod(up);
        attr.velocity = FmodHelper::VectorToFmod(velocity);

        //TODO: Add a multi-listener system.
        CheckFmod(s_Engine->System->setListenerAttributes(0, &attr));
    }

    TypeId Voxaudio::PlaySound(TypeId soundId, const Vector3& pos, float volumedB)
    {
        TypeId channelId = s_Engine->NextChannelId++;

        auto soundIt = s_Engine->Sounds.find(soundId);
        if (soundIt == s_Engine->Sounds.end()) return channelId;

        s_Engine->Channels[channelId] = std::make_unique<Channel>(*s_Engine, soundId, soundIt->second->m_Definition, pos, volumedB);

        return channelId;
    }

    void Voxaudio::StopChannel(TypeId channelId, float fadeTimeSeconds)
    {
        bool isPlaying = false;
        auto channelIt = s_Engine->Channels.find(channelId);
        if(channelIt == s_Engine->Channels.end()) return;

        if(fadeTimeSeconds <= 0.0f)
        {
            channelIt->second->m_StopRequested = true;
//            channelIt->second->m_EventInstance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
        }
        else
        {
            channelIt->second->m_StopRequested = true;
            channelIt->second->m_StopFader.StartFade(0.0f, fadeTimeSeconds);
        }
    }

    void Voxaudio::StopAllChannels()
    {
        for (auto& channel : s_Engine->Channels)
        {
            channel.second->m_EventInstance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
        }
    }

    void Voxaudio::SetChannel3dPosition(TypeId channelId, const Vector3& position)
    {
        auto tFoundIt = s_Engine->Channels.find(channelId);
        if(tFoundIt == s_Engine->Channels.end()) return;

        tFoundIt->second->m_Position = position;
    }

    void Voxaudio::SetChannelVolume(TypeId channelId, float volumedB)
    {
        auto tFoundIt = s_Engine->Channels.find(channelId);
        if(tFoundIt == s_Engine->Channels.end()) return;

        tFoundIt->second->m_VolumedB = Helper::dBToVolume(volumedB);
    }

    bool Voxaudio::IsPlaying(TypeId channelId)
    {
        bool isPlaying = false;
        auto tFoundIt = s_Engine->Channels.find(channelId);
        if(tFoundIt == s_Engine->Channels.end()) return isPlaying;

        isPlaying = tFoundIt->second->IsPlaying();
        return isPlaying;
    }

    OneShotSound Voxaudio::PlayOnShot(const SoundDefinition &soundDef, const Vector3& pos, float volumedB)
    {
        TypeId soundId = RegisterSound(soundDef, false);
        s_Engine->Sounds[soundId]->m_OneShot = true;
        LoadSound(soundId);
        TypeId channelId = PlaySound(soundId, pos, volumedB);
        return {soundId, channelId};
    }
}
