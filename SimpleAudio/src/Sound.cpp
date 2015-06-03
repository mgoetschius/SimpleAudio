#include "Sound.h"
#include "SimpleAudio.h"
#include "WaveFile.h"
#include "OggFile.h"
#include "NullSound.h"
#include <algorithm>

Sound::Sound()
{
    volume = 1;
	isPlaying = false;
	repeating = false;
}

int Sound::Open(const char *filename)
{
    //std::string s = std::string(filename);
    //std::vector<std::string> str = Split(s, '.', true);
	ptr = std::make_shared<WaveFile>();
	if(ptr->Open(filename) != SA_WAVE_GOOD)
	{
		ptr = std::make_shared<OggFile>();
		if(ptr->Open(filename) != SA_OGG_GOOD)
		{
			ptr= std::make_shared<NullSound>();
			return SA_NULLSOUND;
		}
	}
	/*
	if(str.size() > 0)
	{
		if(str[1] == "wav")
		{
			//ptr = new WaveFile();
			ptr = std::make_shared<WaveFile>();
			if(!ptr->Open(filename))
			{
				ptr = std::make_shared<NullSound>();
				return false;
			}
		}
	
		else if(str[1] == "ogg")
		{
			//ptr = new OggFile();
			ptr = std::make_shared<OggFile>();
			if(!ptr->Open(filename))
			{
				ptr = std::make_shared<NullSound>();
				return false;
			}
		}

		else
		{
			ptr = std::make_shared<NullSound>();
			return false;
		}

	}
	else
	{
		ptr = std::make_shared<NullSound>();
		return false;
	}
	*/
	return true;
}

void Sound::Read(int size, void* buffer)
{
    ptr->Read(size, buffer);
}

void Sound::Play()
{
    ptr->Play();
}

void Sound::Stop()
{
    ptr->Stop();
}

void Sound::Reset()
{
    ptr->Reset();
}

bool Sound::GetIsPlaying()
{
   return ptr->GetIsPlaying();
}
void Sound::SetVolume(float vol)
{
    /// volume should be a float between 0  and 1 inclusive
    if(vol > 1)
        volume = 1;
    else if (vol < 0)
        volume = 0;
    else
        volume = vol;
}

float Sound::GetVolume()
{
    return volume;
}

void Sound::SetRepeating(bool repeat)
{
    ptr->SetRepeating(repeat);
}

Sound::~Sound()
{

}

bool isEmptyString(std::string s) {return s.length() == 0;}
std::vector<std::string> Sound::Split(std::string& s, char c, bool removeEmpties)
{

    std::vector<std::string> vstr;
    std::string::size_type i = 0;
    std::string::size_type j = s.find(c);
    while(j != std::string::npos)
    {
        vstr.push_back(s.substr(i, j-i));
        i = ++j;
        j = s.find(c, j);

        if (j == std::string::npos)
		{
         vstr.push_back(s.substr(i, s.length()));
		 return vstr;
		}
    }
    if(removeEmpties)
        vstr.erase(std::remove_if(vstr.begin(), vstr.end(), isEmptyString), vstr.end());

    return vstr;
}


