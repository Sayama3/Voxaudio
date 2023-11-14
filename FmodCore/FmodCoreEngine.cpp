//
// Created by ianpo on 04/11/2023.
//

#include "FmodCoreEngine.hpp"
#include "FileDialogs.hpp"
#include <vector>
#include <filesystem>
#include <yaml-cpp/yaml.h>

#define VIRTUALIZE_FADE_TIME 1.0f
#define SILENCE_dB 0.0f

namespace fs = std::filesystem;

namespace Voxymore::Audio
{
    FmodCoreEngine::FmodCoreEngine(const fs::path& configPath) : NextChannelId(0), NextSoundId(0)
    {
        ConfigPath = configPath;
        if(configPath.empty())
        {
            ConfigPath = FileDialogs::OpenFile({"Voxaudio Config (*.vxm)", "*.vxm"});

            if(configPath.empty())
            {
                ConfigPath = "./VoxaudioConfig.vxm";
            }
        }

        if(fs::exists(ConfigPath)) ReadConfigFile();
        else WriteConfigFile();

        CheckFmod(FMOD::System_Create(&System));
        CheckFmod(System->init(Config.numberOfChannels, FMOD_INIT_NORMAL, nullptr));
    }

    FmodCoreEngine::~FmodCoreEngine()
    {
        CheckFmod(System->release());
    }

    void FmodCoreEngine::Update(float deltaTime)
    {
        std::vector<ChannelMap::iterator> stoppedChannels;
        for (auto it = Channels.begin(), itEnd = Channels.end(); it != itEnd; it++)
        {
            it->second->Update(deltaTime);
            if (it->second->m_State == Channel::State::Stopped)
            {
                stoppedChannels.push_back(it);
            }
        }

        for (auto& it : stoppedChannels)
        {
            Channels.erase(it);
        }

        System->update();
    }

    void FmodCoreEngine::ReadConfigFile()
    {
        YAML::Node config = YAML::LoadFile(ConfigPath.string());
        YAML::Node FmodCoreConfig = config["Voxaudio.FmodCore"];
        if(FmodCoreConfig)
        {
            Config.numberOfChannels = FmodCoreConfig["NumberOfChannels"].as<int>();
        }
    }

    void FmodCoreEngine::WriteConfigFile()
    {
        YAML::Node config = YAML::LoadFile(ConfigPath.string());
        YAML::Node FmodCoreConfig = config["Voxaudio.FmodCore"];
        if(FmodCoreConfig)
        {
            FmodCoreConfig["NumberOfChannels"] = Config.numberOfChannels;
        }
    }

    void FmodCoreEngine::LoadSound(TypeId soundId)
    {
        if(SoundIsLoaded(soundId)) return;

        auto soundIt = Sounds.find(soundId);
        if(soundIt == Sounds.end()) return;

        SoundDefinition& definition = soundIt->second->m_Definition;

        // FMOD_NONBLOCKING = load sound async.
        FMOD_MODE mode = FMOD_NONBLOCKING;
        mode |= definition.is3D ? (FMOD_3D | FMOD_3D_INVERSETAPEREDROLLOFF) : FMOD_2D;
        mode |= definition.isLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
        mode |= definition.isStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

        CheckFmod(System->createSound(definition.name.c_str(), mode, nullptr, &soundIt->second->m_Sound));

        if(soundIt->second->m_Sound)
        {
            CheckFmod(soundIt->second->m_Sound->set3DMinMaxDistance(definition.minDistance, definition.maxDistance));
        }
    }

    void FmodCoreEngine::UnloadSound(TypeId soundId)
    {
        if(!SoundIsLoaded(soundId)) return;

        auto soundIt = Sounds.find(soundId);
        if(soundIt == Sounds.end()) return;

        if(soundIt->second->m_Sound)
        {
            CheckFmod(soundIt->second->m_Sound->release());
        }
    }

    //TODO: Check that this is the correct way to check if a sound is loaded.
    bool FmodCoreEngine::SoundIsLoaded(TypeId soundId) const {
        auto soundIt = Sounds.find(soundId);
        if(soundIt == Sounds.end()) return false;

        return soundIt->second->m_Sound != nullptr;
    }

    Channel::Channel(FmodCoreEngine &engine, TypeId soundId, const SoundDefinition &definition, const Vector3 &position, float volumedB)
        : m_Engine(engine), m_Channel(nullptr), m_SoundId(soundId), m_Position(position), m_VolumedB(volumedB), m_SoundVolume(Helper::dBToVolume(volumedB))
    {
        auto soundIt = m_Engine.Sounds.find(m_SoundId);
        if (soundIt == m_Engine.Sounds.end()) return;

        m_Engine.System->playSound(soundIt->second->m_Sound, nullptr, true, &m_Channel);
        if (m_Channel)
        {
            UpdateChannelParameters();
            m_Channel->setPaused(false);
        }
    }

    Channel::~Channel()
    {
    }

    void Channel::Update(float deltaTime)
    {
        switch (m_State)
        {
            case State::Initialize: [[fallthrough]];
            case State::Devirtualize:
            case State::ToPlay:
            {
                if(m_StopRequested)
                {
                    m_State = State::Stopping;
                    return;
                }

                if(ShouldBeVirtual(true))
                {
                    if(IsOneShot())
                    {
                        m_State = State::Stopping;
                    }
                    else
                    {
                        m_State = State::Virtual;
                    }
                    return;
                }

                if(!m_Engine.SoundIsLoaded(m_SoundId))
                {
                    m_Engine.LoadSound(m_SoundId);
                    m_State = State::Loading;
                    return;
                }

                m_Channel = nullptr;
                auto soundIt = m_Engine.Sounds.find(m_SoundId);
                if(soundIt !=  m_Engine.Sounds.end())
                {
                    m_Engine.System->playSound(soundIt->second->m_Sound, nullptr, true, &m_Channel);
                    if(m_Channel)
                    {
                        if(m_State == State::Devirtualize)
                        {
                            //Fade In for Virtualize
                            m_VirtualizeFader.StartFade(SILENCE_dB, 0.0f, VIRTUALIZE_FADE_TIME);
                        }
                        m_State = State::Playing;

                        FMOD_VECTOR p = FmodHelper::VectorToFmod(m_Position);
                        m_Channel->set3DAttributes(&p, nullptr);
                        m_Channel->setVolume(Helper::dBToVolume(m_VolumedB));
                        m_Channel->setPaused(false);
                    }
                    else
                    {
                        m_State = State::Stopping;
                    }
                }

                break;
            }
            case State::Loading:
            {
                if(m_Engine.SoundIsLoaded(m_SoundId))
                {
                    m_State = State::ToPlay;
                }
                break;
            }
            case State::Playing:
            {
                m_VirtualizeFader.Update(deltaTime);
                // Update everything, the position, the volume, everything...
                UpdateChannelParameters();

                if(!IsPlaying() || m_StopRequested)
                {
                    m_State= State::Stopping;
                    return;
                }

                if(ShouldBeVirtual(false))
                {
                    m_VirtualizeFader.StartFade(SILENCE_dB, VIRTUALIZE_FADE_TIME);
                    m_State = State::Virtualizing;
                }
                break;
            }
            case State::Stopping:
            {
                m_StopFader.Update(deltaTime);
                UpdateChannelParameters();
                if(m_StopFader.IsFinished())
                {
                    m_Channel->stop();
                }
                if(!IsPlaying())
                {
                    m_State = State::Stopped;
                    return;
                }
                break;
            }
            case State::Stopped:
            {
                break;
            }
            case State::Virtualizing:
            {
                m_VirtualizeFader.Update(deltaTime);
                UpdateChannelParameters();
                if(ShouldBeVirtual(false))
                {
                    m_VirtualizeFader.StartFade(0.0f, VIRTUALIZE_FADE_TIME);
                    m_State = State::Playing;
                }
                if(m_VirtualizeFader.IsFinished())
                {
                    m_Channel->stop();
                    m_State = State::Virtual;
                }
                break;
            }
            case State::Virtual:
            {
                if(m_StopRequested)
                {
                    m_State = State::Stopping;
                }
                else if(!ShouldBeVirtual(false))
                {
                    m_State = State::Devirtualize;
                }
                break;
            }
        }
    }

    float Channel::GetVolumedB() const {
        return m_VolumedB;
    }

    bool Channel::IsPlaying() const {
        bool isPlaying = false;
        if(m_Channel == nullptr) return isPlaying;

        m_Channel->isPlaying(&isPlaying);
        return isPlaying;
    }

    void Channel::UpdateChannelParameters()
    {
        if(m_Channel == nullptr) return;

        FMOD_VECTOR p = FmodHelper::VectorToFmod(m_Position);
        m_Channel->set3DAttributes(&p, nullptr);
        m_Channel->setVolume(Helper::dBToVolume(m_VolumedB));
    }

    bool Channel::IsOneShot() const
    {
        auto soundIt = m_Engine.Sounds.find(m_SoundId);
        if (soundIt == m_Engine.Sounds.end()) return false;

        return soundIt->second->m_OneShot;
    }

    // It should be some calculation to see if the sound is still worth playing.
    // Maybe a lookup in the sound defintion to see if the sound is worth virtualizing.
    // Most likely a check of the distance of the sound so when it's above the specified threshold we virtualize it.
    // Currently, only a distance check is done.
    bool Channel::ShouldBeVirtual(bool allowVirtualOneShot) const
    {
        if(!allowVirtualOneShot && IsOneShot()) return false;

        auto defPtr = GetSoundDefinition();
        if (!defPtr) return false;

        FMOD_VECTOR pos;
        FMOD_VECTOR vel;
        FMOD_VECTOR forward;
        FMOD_VECTOR up;
        m_Engine.System->get3DListenerAttributes(0, &pos, &vel, &forward, &up);

        Vector3 listenerPos = FmodHelper::FmodToVector(pos);

        float distance = glm::distance(m_Position, listenerPos);

        return distance > defPtr->maxDistance;
    }


    const SoundDefinition* Channel::GetSoundDefinition() const
    {
        auto soundIt = m_Engine.Sounds.find(m_SoundId);
        if (soundIt == m_Engine.Sounds.end()) return nullptr;
        return &soundIt->second->m_Definition;
    }

    Sound::Sound(const SoundDefinition & def, bool oneShot) : m_Sound(nullptr), m_Definition(def), m_OneShot(oneShot)
    {

    }

    bool AudioFader::IsFinished() const {
        return CurrentTime >= TimeFade;
    }

    void AudioFader::StartFade(float toVolumedB, float fadeTimeSeconds)
    {
        FromVolumedB = GetCurrentVolumedB();
        ToVolumedB = toVolumedB;
        CurrentTime = 0.0f;
        TimeFade = fadeTimeSeconds;
    }

    void AudioFader::StartFade(float fromVolumedB, float toVolumedB, float fadeTimeSeconds)
    {
        FromVolumedB = fromVolumedB;
        ToVolumedB = toVolumedB;
        CurrentTime = 0.0f;
        TimeFade = fadeTimeSeconds;
    }

    void AudioFader::Update(float deltaTimeSeconds)
    {
        CurrentTime += deltaTimeSeconds;
    }

    float AudioFader::GetCurrentVolumedB() const {
        return std::lerp(FromVolumedB, ToVolumedB, std::clamp(CurrentTime/TimeFade, 0.0f, 1.0f));
    }

    float AudioFader::GetCurrentVolume() const {
        return Helper::dBToVolume(GetCurrentVolumedB());
    }

    FMOD_VECTOR FmodHelper::VectorToFmod(const Vector3& v)
    {
        FMOD_VECTOR fv;
        fv.x = v.x;
        fv.y = v.y;
        fv.z = v.z;
        return fv;
    }

    Vector3 FmodToVector(const FMOD_VECTOR& fv)
    {
        Vector3 v;
        v.x = fv.x;
        v.y = fv.y;
        v.z = fv.z;
        return v;
    }
}
