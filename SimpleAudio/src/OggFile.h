#ifndef OGGFILE_H
#define OGGFILE_H

#include "Sound.h"
#include "stb_vorbis.h"


class OggFile : public Sound
{
    public:
        OggFile();
        int Open(const char *filename);
        void Read(int size, void* buffer);
        void Play();
        void Stop();
        void Reset();
        bool GetIsPlaying() {return isPlaying;};
        float GetVolume() {return volume;};
        void SetVolume(float vol);
        void SetRepeating(bool repeat) {repeating = repeat;};
        virtual ~OggFile();
    protected:
    private:
        stb_vorbis *v;
        unsigned int channels;
        unsigned int sampleRate;
};

#endif // OGGFILE_H
