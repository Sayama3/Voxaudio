#include "Voxaudio.hpp"
#include "FmodCoreEngine.hpp"
#include <fmod.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Voxymore::Audio
{
    float Helper::dBToVolume(float dB)
    {
        return std::pow(10.0f, 0.05f * dB);
    }
    float Helper::VolumeTodB(float Volume)
    {
        return 20.0f * std::log10(Volume);
    }

    FmodCoreEngine* s_Engine = nullptr;

    void Voxaudio::Init(const std::filesystem::path& configFile)
    {
        s_Engine = new FmodCoreEngine(configFile);
    }
    void Voxaudio::Update(float deltaTimeSeconds)
    {
        s_Engine->Update();
    }
    void Voxaudio::Shutdown()
    {
        delete s_Engine;
    }

    int Voxaudio::RegisterSound(const SoundDefinition& soundDef, bool load)
    {

    }
    void Voxaudio::UnregisterSound(int soundId)
    {

    }
    void Voxaudio::LoadSound(int soundId)
    {

    }
    void Voxaudio::UnloadSound(int soundId)
    {

    }
/*
    void Voxaudio::LoadSound(const std::string& name, bool is3D, bool isLooping, bool isStream)
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
        s_Engine->System->createSound(name.c_str(), mode, nullptr, &soundPtr);
        if (soundPtr)
        {
            s_Engine->Sounds[name] = soundPtr;
        }
    }

    void Voxaudio::UnloadSound(const std::string& name)
    {
        auto soundIt = s_Engine->Sounds.find(name);
        if (soundIt == s_Engine->Sounds.end()) return;
        soundIt->second->release();
        s_Engine->Sounds.erase(soundIt);
    }
*/
    void Voxaudio::Set3dListenerAndOrientation(const Vector3& position, const Vector3& look, const Vector3& up)
    {
        auto fmodPos = FmodHelper::VectorToFmod(position);
        auto fmodLook = FmodHelper::VectorToFmod(look);
        auto fmodUp = FmodHelper::VectorToFmod(up);
        //TODO: Add a multi-listener system.
        s_Engine->System->set3DListenerAttributes(0, &fmodPos, nullptr, &fmodLook, &fmodUp);
    }

    void Voxaudio::Set3dListenerAndOrientation(const Vector3& position, const Vector3& look, const Vector3& up, const Vector3& velocity)
    {
        auto fmodPos = FmodHelper::VectorToFmod(position);
        auto fmodLook = FmodHelper::VectorToFmod(look);
        auto fmodUp = FmodHelper::VectorToFmod(up);
        auto fmodVel = FmodHelper::VectorToFmod(velocity);
        //TODO: Add a multi-listener system.
        s_Engine->System->set3DListenerAttributes(0, &fmodPos, &fmodVel, &fmodLook, &fmodUp);
    }

    int Voxaudio::PlaySound(const std::string& name, const Vector3& pos, float volumedB)
    {
        int channelId = s_Engine->NextChannelId++;

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
        s_Engine->System->playSound(soundIt->second, nullptr, true, &channel);
        if (channel)
        {
            auto position = FmodHelper::VectorToFmod(pos);
            channel->set3DAttributes(&position, nullptr);
            channel->setVolume(Helper::dBToVolume(volumedB));
            channel->setPaused(false);
            s_Engine->Channels[channelId] = channel;
        }

        return channelId;
    }

    void Voxaudio::StopChannel(int channelId)
    {
        bool isPlaying = false;
        auto tFoundIt = s_Engine->Channels.find(channelId);
        if(tFoundIt == s_Engine->Channels.end()) return;

        tFoundIt->second->stop();
    }

    void Voxaudio::StopAllChannels()
    {
        for (auto it = s_Engine->Channels.begin(); it != s_Engine->Channels.end() ; it++)
        {
            it->second->stop();
        }
    }

    void Voxaudio::SetChannel3dPosition(int channelId, const Vector3& position)
    {
        auto tFoundIt = s_Engine->Channels.find(channelId);
        if(tFoundIt == s_Engine->Channels.end()) return;

        auto fmodVec = FmodHelper::VectorToFmod(position);
        tFoundIt->second->set3DAttributes(&fmodVec, nullptr);
    }

    void Voxaudio::SetChannelVolume(int channelId, float volumedB)
    {
        auto tFoundIt = s_Engine->Channels.find(channelId);
        if(tFoundIt == s_Engine->Channels.end()) return;

        tFoundIt->second->setVolume(Helper::dBToVolume(volumedB));
    }

    bool Voxaudio::IsPlaying(int channelId) const
    {
        bool isPlaying = false;
        auto tFoundIt = s_Engine->Channels.find(channelId);
        if(tFoundIt == s_Engine->Channels.end()) return isPlaying;

        tFoundIt->second->isPlaying(&isPlaying);
        return isPlaying;
    }

}