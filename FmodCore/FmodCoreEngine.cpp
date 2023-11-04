#include "FmodCoreEngine.hpp"
#include "FileDialogs.hpp"
#include <vector>
#include <filesystem>
#include <yaml-cpp/yaml.h>

namespace fs = std::filesystem;

namespace Voxymore::Audio
{
	FmodCoreEngine::FmodCoreEngine(const fs::path& configPath)
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

	void FmodCoreEngine::Update()
	{
		std::vector<ChannelMap::iterator> stoppedChannels;
		for (auto it = Channels.begin(), itEnd = Channels.end(); it != itEnd; it++)
		{
			bool isPlaying;
			it->second->isPlaying(&isPlaying);
			if (!isPlaying)
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

    FMOD_VECTOR FmodHelper::VectorToFmod(const Vector3& v)
    {
        FMOD_VECTOR fv;
        fv.x = v.x;
        fv.y = v.y;
        fv.z = v.z;
        return fv;
    }

}