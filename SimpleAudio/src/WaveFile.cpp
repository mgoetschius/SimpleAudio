#include "WaveFile.h"
#include "SimpleAudio.h"

WaveFile::WaveFile()
{

}

int WaveFile::Open(const char *filename)
{
    in = new std::ifstream(filename, std::ios::binary | std::ios::in);

    if(!in)
    {
        return SA_OPEN_ERROR;
    }
    //read data into headerChunk;
    in->seekg(0, std::ios::beg);
    in->read(reinterpret_cast<char*>(&headerChunk), sizeof(HeaderChunk));
    if(headerChunk.id != 1179011410 || headerChunk.waveId != 1163280727)  /// binary numbers for 'FFIR' and 'EVAW'
    {
        return SA_WAVE_FORMAT_ERROR;
    }
    /// search for fmt Chunk
    bool found = false;
    int i = 11;             ///starting search at byte ++11, expected find at 12;  i incremented before use in search
    long holder;
    do
    {
        in->seekg(++i, std::ios::beg);
        in->read(reinterpret_cast<char*>(&holder), sizeof(long));
        if(holder == 544501094)     /// 544501094 binary for ' tmf'
        {
            found = true;
            in->seekg(i, std::ios::beg);
            in->read(reinterpret_cast<char*>(&fmtChunk), sizeof(fmtChunk));
        }
        if(i > headerChunk.fileSize)
        {
            return SA_WAVE_FORMAT_ERROR;
        }
    } while (!found);
    /// search for data chunk
    found = false;
    i = 35;                 ///starting search at byte ++35, expected find at 36;  i incremented before use in search
    do
    {
        in->seekg(++i, std::ios::beg);
        in->read(reinterpret_cast<char*>(&holder), sizeof(holder));
        if(holder == 1635017060)    /// 1635017060 is 'atad' = little endian data
        {
            found = true;
            waveDataStartLoc = i;
            in->seekg(i, std::ios::beg);
            in->read(reinterpret_cast<char*>(&dataChunk), sizeof(dataChunk));
        }
        if(i > headerChunk.fileSize)
        {
            return SA_WAVE_FORMAT_ERROR;
        }
    } while(!found);

    /// get to the start of the raw wave data
    in->seekg(waveDataStartLoc+8, std::ios::beg);
    curLocation = in->tellg();

    /// set some class variables
    isPlaying = false;
    repeating = false;
    volume = 1;

	return SA_WAVE_GOOD;
}

void WaveFile::Read(int size, void *buffer)
{
    in->seekg(curLocation, std::ios::beg);
    in->read((char*)buffer, size);
    curLocation += size;
    if(in->gcount() < size)
    {
        if(in->gcount() == 0)
        {
            if(!repeating)
                isPlaying = false;
        }
        else
        {
            /// if buffer isn't filled, fill the remaider with zeros
            memset((char*)buffer + in->gcount(), 0, size - (int)in->gcount());
            if(!repeating)
                isPlaying = false;
        }

        /// reset curLocation to beginning of the data chunk;
        Reset();
    }
}

void WaveFile::Play()
{
    isPlaying = true;
}

void WaveFile::Stop()
{
    isPlaying = false;
}

void WaveFile::Reset()
{
    in->clear();
    in->seekg(waveDataStartLoc+8, std::ios::beg);
    curLocation = in->tellg();
}

WaveFile::~WaveFile()
{
	in->close();
	delete in;
}
