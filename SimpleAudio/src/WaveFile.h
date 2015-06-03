#ifndef WAVEFILE_H
#define WAVEFILE_H

#include <fstream>
#include <windows.h>
#include "Sound.h"

class WaveFile : public Sound
{
    public:
        WaveFile();
        int Open(const char *filename);
        void Read(int size, void* buffer);
        void Play();
        void Stop();
        void Reset();
        bool GetIsPlaying() {return isPlaying;};
        void SetRepeating(bool repeat) {repeating = repeat;};
        virtual ~WaveFile();


    protected:
    private:
        std::ifstream *in;

        int waveDataStartLoc;
        std::streamoff curLocation;
        struct HeaderChunk
        {
            long id;
            long fileSize;
            long waveId;
        };
        struct FmtChunk
        {
            long id;
            long chunkSize;
            short format;
            short numChannels;
            long sampleRate;
            long avgBytesPerSec;
            short blockAlign;
            short bitsPerSample;
            short cbSize;
        };
        struct DataChunk
        {
            long id;
            long chunkSize;
        };

        HeaderChunk headerChunk;
        FmtChunk fmtChunk;
        DataChunk dataChunk;

};

#endif // WAVEFILE_H
