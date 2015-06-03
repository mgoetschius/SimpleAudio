#ifndef NULLSOUND_H
#define NULLSOUND_H

#include "sound.h"

class NullSound : public Sound
{
public:
	NullSound(void);
	int Open(const char *filename);
    void Read(int size, void* buffer);
    void Play();
    void Stop();
    void Reset();
    bool GetIsPlaying() {return isPlaying;};
    void SetRepeating(bool repeat) {repeating = repeat;};
	virtual ~NullSound(void);
};

#endif