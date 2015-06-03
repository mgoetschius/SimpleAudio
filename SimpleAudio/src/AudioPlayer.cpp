#include "SimpleAudio.h"
#include "AudioPlayer.h"
#include "WinMMPlayer.h"
#include "XAudio2Player.h"
#include "NullPlayer.h"

AudioPlayer::AudioPlayer()
{
	
}

std::shared_ptr<AudioPlayer> AudioPlayer::Open(int &status)
{
	
	{
		std::shared_ptr<AudioPlayer> player(XAudio2Player::Open(status));
		if(player != 0)
		{
			status = SA_XAUDIO2PLAYER_STARTED;
			return player;
		}
	}
	
	{
		std::shared_ptr<AudioPlayer> player(WinMMPlayer::Open());
		if(player != 0)
		{
			status = SA_WINMMPLAYER_STARTED;
			return player;
		}
	}
	
	std::shared_ptr<AudioPlayer> player(new NullPlayer);
	status = SA_NULLPLAYER;
	return player;
}

void AudioPlayer::AddSound(Sound &s)
{
	std::lock_guard<std::mutex> guard(mutex);
    sounds.push_back(&s);
}

void AudioPlayer::Play()
{
   
}

void AudioPlayer::CleanUp()
{

}

AudioPlayer::~AudioPlayer()
{
}
