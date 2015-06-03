#include "SimpleAudio.h"
#include "XAudio2Player.h"

XAudio2Player* XAudio2Player::Open(int &status)
{
	CoInitializeEx( NULL, COINIT_MULTITHREADED );
	IXAudio2* pxa = NULL;

	HRESULT hr;
	if( FAILED(hr = XAudio2Create( &pxa, 0, XAUDIO2_DEFAULT_PROCESSOR ) ) )
	{
		status = SA_XAUDIO2_ERROR;
		return 0;
	}
	else
	{
		status = SA_NO_ERROR;
		return new XAudio2Player(pxa, status);
	}
}

XAudio2Player::XAudio2Player(IXAudio2* pxa, int &status)
{
	
	currentBufferNumber = 0;
	
	pXAudio2 = pxa;
	
	HRESULT hr;
	if( FAILED(hr = XAudio2Create( &pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR ) ) )
		status = SA_XAUDIO2_ERROR;
	IXAudio2MasteringVoice* pMasterVoice = NULL;
	if ( FAILED(hr = pXAudio2->CreateMasteringVoice( &pMasterVoice ) ) )
		status = SA_XAUDIO2_ERROR;

	wf.wFormatTag = WAVE_FORMAT_PCM;
    wf.nChannels = 2;
    wf.nSamplesPerSec = 44100;
    wf.nAvgBytesPerSec = 44100 * 4;
    wf.nBlockAlign = 4;
    wf.wBitsPerSample = 16;
    wf.cbSize = 0;

	if( FAILED(hr = pXAudio2->CreateSourceVoice( &pSourceVoice, &wf, 0, 1.0f, &voiceContext ) ) ) 
		status = SA_XAUDIO2_ERROR;
	if ( FAILED(hr = pSourceVoice->Start( 0, 0 ) ) )
		status = SA_XAUDIO2_ERROR;
	
	doUpdate = true;

	t = std::thread(&XAudio2Player::Play, this);
	
}

XAudio2Player::XAudio2Player(void)
{
	
	currentBufferNumber = 0;
	
	CoInitializeEx( NULL, COINIT_MULTITHREADED );
	
	pXAudio2 = NULL;
	
	HRESULT hr;
	if( FAILED(hr = XAudio2Create( &pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR ) ) )
		return;
	IXAudio2MasteringVoice* pMasterVoice = NULL;
	if ( FAILED(hr = pXAudio2->CreateMasteringVoice( &pMasterVoice ) ) )
		return;
	
	wf.wFormatTag = WAVE_FORMAT_PCM;
    wf.nChannels = 2;
    wf.nSamplesPerSec = 44100;
    wf.nAvgBytesPerSec = 44100 * 4;
    wf.nBlockAlign = 4;
    wf.wBitsPerSample = 16;
    wf.cbSize = 0;

	if( FAILED(hr = pXAudio2->CreateSourceVoice( &pSourceVoice, &wf, 0, 1.0f, &voiceContext ) ) ) 
		return;
	if ( FAILED(hr = pSourceVoice->Start( 0, 0 ) ) )
		return;
	
	doUpdate = true;
	
	t = std::thread(&XAudio2Player::Play, this);
	
}

void XAudio2Player::Play()
{
	
	short readBuffer[SIZE];
	short mixBuffer[SIZE];
	memset(mixBuffer, 0, SIZE);
	bool oneIsPlaying = false;
	bool last = true;

	while(doUpdate)
	{
		XAUDIO2_VOICE_STATE state;
		pSourceVoice->GetState(&state);
		while( pSourceVoice->GetState( &state ), state.BuffersQueued >= BLOCKS - 1)
		{
			WaitForSingleObject( voiceContext.hBufferEndEvent, INFINITE );
		}
		if(state.BuffersQueued < BLOCKS - 1)
		{
			oneIsPlaying = false;
			std::lock_guard<std::mutex> guard(mutex);
			for(std::vector<Sound*>::iterator i = sounds.begin(); i != sounds.end(); i++)
            {
				if((*i)->GetIsPlaying())
                {
					oneIsPlaying = true;
                    (*i)->Read(SIZE, readBuffer);
                    for(int j = 0; j < SIZE; j++)
                    {
                        int result = (int)(mixBuffer[j] + readBuffer[j] * (*i)->GetVolume());
                        if(result < 32767
                            && result > -32767)
                            mixBuffer[j] += (int)(readBuffer[j] * (*i)->GetVolume());
                        else
                        {
                            // don't clip by rolling over the adding the sounds
                            if(result > 32767)
                                mixBuffer[j] = 32767;
                            if(result < -32767)
                                mixBuffer[j] = -32767;
                        }
                    }
					
                }
			} // end for loop
			if(oneIsPlaying)
			{
				memcpy(dataBuffer[currentBufferNumber], mixBuffer, SIZE);
				XAUDIO2_BUFFER xBuffer = {0};
				xBuffer.AudioBytes = SIZE;  //buffer containing audio data
				xBuffer.pAudioData = (BYTE*)dataBuffer[currentBufferNumber];  //size of the audio buffer in bytes
				last = true;
				for(std::vector<Sound*>::iterator i = sounds.begin(); i != sounds.end(); i++)
				{
					if((*i)->GetIsPlaying())
						last = false;

				}
				if(last)
					xBuffer.Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer
				HRESULT hr;
				if( FAILED(hr = pSourceVoice->SubmitSourceBuffer( &xBuffer ) ) )
					hr = pSourceVoice->SubmitSourceBuffer( &xBuffer );
			}
		}
		memset(mixBuffer, 0, SIZE); // reset buffer to zeros;
		currentBufferNumber++;
		currentBufferNumber %= BLOCKS;
	}
	
}

void XAudio2Player::CleanUp()
{
	doUpdate = false;
	// send end message here.
	short buffer[] = {0};
	XAUDIO2_BUFFER xBuffer = {0};
	xBuffer.AudioBytes = 1;  //buffer containing audio data
	xBuffer.pAudioData = (BYTE*)buffer;  //size of the audio buffer in bytes
	xBuffer.Flags = XAUDIO2_END_OF_STREAM;
	HRESULT hr;
	if( FAILED(hr = pSourceVoice->SubmitSourceBuffer( &xBuffer ) ) )
		hr = pSourceVoice->SubmitSourceBuffer( &xBuffer );
	XAUDIO2_VOICE_STATE state;
	while( pSourceVoice->GetState( &state ), state.BuffersQueued > 0)
	{
		WaitForSingleObject( voiceContext.hBufferEndEvent, INFINITE );
	}
	
	t.join();
	
    /// empty sounds vector
    sounds.clear();

}


XAudio2Player::~XAudio2Player(void)
{
	pXAudio2->Release();
	CoUninitialize();
}
