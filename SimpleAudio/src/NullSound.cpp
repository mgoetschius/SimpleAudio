#include "NullSound.h"


NullSound::NullSound(void)
{
	
}

int NullSound::Open(const char *filename)
{
   return true;
}


void NullSound::Read(int size, void *buffer)
{
    
}

void NullSound::Play()
{
    
}

void NullSound::Stop()
{
   
}

void NullSound::Reset()
{
    
}


NullSound::~NullSound(void)
{
}
