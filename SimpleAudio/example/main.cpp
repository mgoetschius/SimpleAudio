#include <iostream>

#include "SimpleAudio.h"

int main(void)
{
	{
		std::cout << "Simple Audio" << std::endl;

		int status = 0;

		// Declare a player then open the player
		SAPlayer player;
		// Returns a smart pointer to the the player it finds, either XAudio2,
		// WinMM, or NULL.  Just let it go out of scope when you're done with it.
		player = AudioPlayer::Open(status);


		// Declare a sound, then add it to the player
		// Let the sound go out of scope when you're done with it.
		Sound s;
		s.Open("test.ogg");
		s.SetVolume(.2f);		// values from 0.0 to 1.0
		player->AddSound(s);
		// to play the sound.
		s.Play();

		while(s.GetIsPlaying())
		{
			std::cout << "|";
			Sleep(100);
			std::cout << '\b' << "/";
			Sleep(100);
			std::cout << '\b' << "-";
			Sleep(100);
			std::cout << '\b' << "\\";
			Sleep(100);
			std::cout << '\b' << "|";
			Sleep(100);
			std::cout << '\b' << "/";
			Sleep(100);
			std::cout << '\b' << "-";
			Sleep(100);
			std::cout << '\b' << "\\";
			Sleep(100);
			std::cout << '\b';

		}
		std::cout << '\b' << std::endl;

		// Stop the player, join the thread, and clear the player's sounds vector
		player->CleanUp();

		std::cout << "Press Enter to end." << std::endl;
		//std::getchar();
	}

	return 0;
}
