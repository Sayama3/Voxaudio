#include "FmodCoreEngine.hpp"

namespace Voxymore::Audio
{
	FmodCoreEngine::FmodCoreEngine()
	{
		//TODO: Init Fmod.
		//TODO: Recover previously saved data.
	}

	FmodCoreEngine::~FmodCoreEngine()
	{
		//TODO: Destroy Fmod.
	}

	void FmodCoreEngine::Update()
	{
		std::vector<ChannelMap::iterator> stoppedChannels;
		for (auto it = Channels.begin(), itEnd = Channels.end(); it != itEnd; it++)
		{
			bool isPlaying;
			channel->isPlaying(&isPlaying);
			if (!isPlaying)
			{
				stoppedChannels.push_back(it);
			}
		}

		for (auto& it : stoppedChannels)
		{
			Channels.erase(it);
		}
	}
}