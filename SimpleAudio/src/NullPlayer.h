#ifndef NULLPLAYER_H
#define NULLPLAYER_H

#include "AudioPlayer.h"

class NullPlayer : public AudioPlayer
{
    public:
        NullPlayer();
        void Play();
        void CleanUp();
        virtual ~NullPlayer();
    protected:
    private:
};

#endif // NULLPLAYER_H
