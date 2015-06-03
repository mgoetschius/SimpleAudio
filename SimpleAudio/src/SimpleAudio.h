// SimpleAudio - a simple lib to play .wav and .ogg files
//
// Written by Mike Goetschius
//
// No waranty is expressed or implied
// Use however you want

#ifndef SIMPLEAUDIO_H
#define SIMPLEAUDIO_H

#include <memory>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <Windows.h>




#define SAPlayer std::shared_ptr<AudioPlayer>

enum SA_Status
{
	SA_NO_ERROR = 0,
	SA_XAUDIO2_ERROR,
	SA_XAUDIO2PLAYER_STARTED,
	SA_WINMMPLAYER_STARTED,
	SA_NULLPLAYER,
	SA_OPEN_ERROR,
	SA_WAVE_FORMAT_ERROR,
	SA_WAVE_GOOD,
	SA_OGG_GOOD,
	SA_NULLSOUND
};

class Sound
{
    public:
        Sound();
        virtual ~Sound();
        virtual int Open(const char *filename);
        virtual void Read(int size, void* buffer);
        virtual void Reset();
        virtual void Play();
        virtual void Stop();
        virtual bool GetIsPlaying();
        void SetVolume(float vol);
        float GetVolume();
        virtual void SetRepeating(bool repeat);
    protected:
        bool isPlaying;
        bool repeating;
        float volume;
    private:
        std::shared_ptr<Sound> ptr;
        std::vector<std::string> Split(std::string& s, char c, bool removeEmpties);

};

class AudioPlayer
{
	public:
		AudioPlayer(void);
		static std::shared_ptr<AudioPlayer> Open(int &status);
		virtual void Play();
		void AddSound(Sound &s);
		virtual void CleanUp();
		virtual ~AudioPlayer();
	protected:
		bool doUpdate;
		std::thread t;
		std::vector<Sound*> sounds;
		static const int BLOCKS = 5;
		static const int SIZE = 4092;
		WAVEFORMATEX wf;
		std::mutex mutex;
    private:
};

#endif