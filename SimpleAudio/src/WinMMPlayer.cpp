#include "WinMMPlayer.h"

WinMMPlayer* WinMMPlayer::Open()
{
	WAVEFORMATEX wf;
	HWAVEOUT hwo;

	wf.wFormatTag = WAVE_FORMAT_PCM;
    wf.nChannels = 2;
    wf.nSamplesPerSec = 44100;
    wf.nAvgBytesPerSec = 44100 * 4;
    wf.nBlockAlign = 4;
    wf.wBitsPerSample = 16;
    wf.cbSize = 0;

    if(MMSYSERR_NOERROR == waveOutOpen(&hwo,WAVE_MAPPER,&wf,0,0,0))
	{
		return new WinMMPlayer(hwo);
	}
	else
	{
		return 0;
	}


}

WinMMPlayer::WinMMPlayer(HWAVEOUT hwo)
{
	hWaveOut = hwo;

	for(int i = 0; i < BLOCKS; i++)
    {
        WAVEHDR &temp = wh[i];
        memset(&temp, 0, sizeof(temp));
        temp.lpData = buffer + i * SIZE;
        temp.dwBufferLength = SIZE;
        temp.dwFlags = 0;
        memset(temp.lpData, 0, SIZE);
        waveOutPrepareHeader(hWaveOut, &temp, sizeof(temp));
        waveOutWrite(hWaveOut, &temp, sizeof(temp));
    }

	doUpdate = true;

    /// start a thread that continuously pumps data into the buffers
    /// runs until CleanUp() is called;
    t = std::thread(&WinMMPlayer::Play, this);
}

WinMMPlayer::WinMMPlayer()
{
    wf.wFormatTag = WAVE_FORMAT_PCM;
    wf.nChannels = 2;
    wf.nSamplesPerSec = 44100;
    wf.nAvgBytesPerSec = 44100 * 4;
    wf.nBlockAlign = 4;
    wf.wBitsPerSample = 16;
    wf.cbSize = 0;

    waveOutOpen(&hWaveOut,WAVE_MAPPER,&wf,0,0,0);

    /// initialize buffers with their memory locations

    for(int i = 0; i < BLOCKS; i++)
    {
        WAVEHDR &temp = wh[i];
        memset(&temp, 0, sizeof(temp));
        temp.lpData = buffer + i * SIZE;
        temp.dwBufferLength = SIZE;
        temp.dwFlags = 0;
        memset(temp.lpData, 0, SIZE);
        waveOutPrepareHeader(hWaveOut, &temp, sizeof(temp));
        waveOutWrite(hWaveOut, &temp, sizeof(temp));
    }

	doUpdate = true;

    /// start a thread that continuously pumps data into the buffers
    /// runs until CleanUp() is called;
    t = std::thread(&WinMMPlayer::Play, this);
}

void WinMMPlayer::Play()
{
    while(doUpdate)
    {
        for(int i = 0; i < BLOCKS; i++)
        {
            WAVEHDR &temp = wh[i];
            if(temp.dwFlags & WHDR_DONE)
            {
               waveOutUnprepareHeader(hWaveOut, &temp, sizeof(temp));

               short mixBuffer[SIZE];
               memset(mixBuffer, 0, SIZE);
               short readBuffer[SIZE];

               /********************************************
               // the following section loops through all sounds,
               // if they are playing, they are added to the mix
               // the mix is very simple
               ***********************************************/
			    std::lock_guard<std::mutex> guard(mutex);
                for(std::vector<Sound*>::iterator i = sounds.begin(); i != sounds.end(); i++)
                {
                    if((*i)->GetIsPlaying())
                    {
                        (*i)->Read(SIZE, readBuffer);
                        for(int j = 0; j < SIZE; j++)
                        {
                            int result = (int)(mixBuffer[j] + readBuffer[j] * (*i)->GetVolume());
                            if(result < 32767
                               && result > -32767)
                                mixBuffer[j] += (int)(readBuffer[j] * (*i)->GetVolume());
                            else
                            {
                                // don't clip by rolling over min/max short values adding the sounds
                                if(result > 32767)
                                    mixBuffer[j] = 32767;
                                if(result < -32767)
                                    mixBuffer[j] = -32767;
                            }
                        }
                    }

                }
                memcpy(temp.lpData, mixBuffer, SIZE);
                temp.dwFlags = 0;
                temp.dwBufferLength = SIZE;
                waveOutPrepareHeader(hWaveOut, &temp, sizeof(temp));
                waveOutWrite(hWaveOut, &temp, sizeof(temp));
            }
        }
    }
}

void WinMMPlayer::CleanUp()
{
    doUpdate = false;

    t.join();

    /// empty sounds vector
    sounds.clear();
}

WinMMPlayer::~WinMMPlayer()
{
}
