This is a library I put together to play sounds in my games and other stuff I'm working on.  Currently it will play .wav files and .ogg files.  It supports either XAudio2 or WinMM to play the sounds.  The lib tries to find an XAudio2 player first, then tries to start a WinMM player, then, if neither is found, a NullPlayer is returned that plays no sound.

Multiple sounds can be played at the same time and are mixed in the software.  However, they must be:
-44,100 sample per sec
-2 channels
-16 bits per sample
No resampling is done in software.

The wave file reader was written by me.  To decode ogg files, Sean Barrett's stb_vorbis is used (https://github.com/nothings/stb/blob/master/stb_vorbis.c).

Use this for whatever you want, however you want.

When compiling with g++, you must use thirdparty/include files in your search path.  If you use Visual Studio, you can use the files from the Window's Kit that are probably already set up by default.
Included are a Code::Blocks project and a VS2012 project.

Dependencies:
Winmm.lib
ole32.lib