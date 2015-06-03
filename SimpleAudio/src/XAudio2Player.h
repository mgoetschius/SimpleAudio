#ifndef XAUDIO2PLAYER_H
#define XAUDIO2PLAYER_H

#include "SimpleAudio.h"
#include <Windows.h>
#include <XAudio2.h>


class XAudio2Player : public AudioPlayer
{
public:
	XAudio2Player(void);
	XAudio2Player(IXAudio2* pxa, int &status);
	static XAudio2Player* Open(int &status);
	void Play();
    void CleanUp();
	~XAudio2Player(void);

	struct StreamingVoiceContext : public IXAudio2VoiceCallback
	{
		STDMETHOD_( void, OnVoiceProcessingPassStart )( UINT32 )
		{
		}
		STDMETHOD_( void, OnVoiceProcessingPassEnd )()
		{
		}
		STDMETHOD_( void, OnStreamEnd )()
		{
		}
		STDMETHOD_( void, OnBufferStart )( void* )
		{
		}
		STDMETHOD_( void, OnBufferEnd )( void* )
		{
			SetEvent( hBufferEndEvent );
		}
		STDMETHOD_( void, OnLoopEnd )( void* )
		{
		}
		STDMETHOD_( void, OnVoiceError )( void*, HRESULT )
		{
		}

		HANDLE hBufferEndEvent;

		StreamingVoiceContext() : hBufferEndEvent( CreateEvent( NULL, FALSE, FALSE, NULL ) )
		{
		}
		virtual ~StreamingVoiceContext()
		{
			CloseHandle( hBufferEndEvent );
		}
	};


private:
	int currentBufferNumber;
	char dataBuffer[BLOCKS][SIZE];
	IXAudio2* pXAudio2;
	StreamingVoiceContext voiceContext;
	IXAudio2SourceVoice* pSourceVoice;
	WAVEFORMATEX wf;

};

#endif