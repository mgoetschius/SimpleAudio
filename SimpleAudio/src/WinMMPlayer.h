#ifndef WINMMPLAYER_H
#define WINMMPLAYER_H

#include "SimpleAudio.h"

class WinMMPlayer : public AudioPlayer
{
    public:
        WinMMPlayer();
		WinMMPlayer(HWAVEOUT hWaveOut);
		static WinMMPlayer* Open();
        void Play();
        void CleanUp();
        virtual ~WinMMPlayer();
    protected:
    private:
        HWAVEOUT hWaveOut;
        WAVEHDR wh[BLOCKS];
        char buffer[BLOCKS * SIZE];
};

#endif // WINMMPLAYER_H
