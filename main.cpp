#include <iostream>
#include "Chip8.h"

const int g_WindowWidth{1000};
const int g_WindowHeight{ g_WindowWidth / 2 };

Chip8 myChip8{ g_WindowWidth, g_WindowHeight};

#undef main
int main(int, char* [])
{	
	myChip8.InitializeGraphics("Chip8 Emulator");
	
	//Initialize the Chip8 system and load the game into memory
	myChip8.Initialize();
	myChip8.LoadGame("D:/School/Portfolio/Chip8_Emulator/Chip8/Chip8Emulator/Resources/pong.rom");

	while(true)
	{
		myChip8.EmulateCycle();
		myChip8.DrawGraphics();
		myChip8.SetKeys();
	}
}
