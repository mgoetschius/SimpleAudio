#include "OggFile.h"
#include "SimpleAudio.h"

OggFile::OggFile()
{
    //ctor
}

int OggFile::Open(const char *filename)
{
    int error = 0;
    v = stb_vorbis_open_filename(filename, &error, NULL);
	if(error != 0)
		return SA_OPEN_ERROR;
    stb_vorbis_info info = stb_vorbis_get_info(v);
    channels = info.channels;
    sampleRate = info.sample_rate;
    isPlaying = false;
    repeating = false;
    volume = 1;

	return SA_OGG_GOOD;
}

void OggFile::Read(int size, void* buffer)
{
    int n;
    /// size/channels because it's number of samples, not size of the buffer
    n = stb_vorbis_get_samples_short_interleaved(v, channels, (short*)buffer, size/channels);
    if(n == 0)
    {
        /// need to figure out how to fill left-over buffer with zeros
        if(!repeating)
            isPlaying = false;

        Reset();
    }

}

void OggFile::Play()
{
    isPlaying = true;
}

void OggFile::Stop()
{
    isPlaying = false;
}

void OggFile::Reset()
{
    stb_vorbis_seek_start(v);
}

void OggFile::SetVolume(float vol)
{
    volume = vol;
}

OggFile::~OggFile()
{
    stb_vorbis_close(v);
}
