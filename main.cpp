#include <iostream>
#include <SDL.h>
#include "Chip8.h"

Chip8 myChip8;

const int g_WindowWidth{1000};
const int g_WindowHeight{ g_WindowWidth / 2 };
void DrawGraphics(const Chip8&, SDL_Renderer*);

int GetOpenGLDriverIndex()
{
	auto openglIndex = -1;
	const auto driverCount = SDL_GetNumRenderDrivers();
	for (auto i = 0; i < driverCount; i++)
	{
		SDL_RendererInfo info;
		if (!SDL_GetRenderDriverInfo(i, &info))
			if (!strcmp(info.name, "opengl"))
				openglIndex = i;
	}
	return openglIndex;
}

int main()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
	{
		throw std::runtime_error(std::string("SDL_Init Error: ") + SDL_GetError());
	}

	const auto window = SDL_CreateWindow(
		"Programming 4 assignment",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		g_WindowWidth,
		g_WindowHeight,
		SDL_WINDOW_OPENGL
	);
	if (window == nullptr)
	{
		throw std::runtime_error(std::string("SDL_CreateWindow Error: ") + SDL_GetError());
	}

	const auto* renderer = SDL_CreateRenderer(window, GetOpenGLDriverIndex(), SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr)
	{
		throw std::runtime_error(std::string("SDL_CreateRenderer Error: ") + SDL_GetError());
	}
	
	//Initialize the Chip8 system and load the game into memory
	myChip8.Initialize();
	myChip8.LoadGame("D:/School/Portfolio/Chip8_Emulator/Chip8/Chip8Emulator/Resources/pong.rom");

	while(true)
	{
		myChip8.EmulateCycle();

		if (myChip8.DrawFlag())
			DrawGraphics(myChip8, SDL_GetRenderer(window));

		myChip8.SetKeys();
	}
}

void DrawGraphics(const Chip8& chip8, SDL_Renderer* pRenderer)
{
	int bufferWidth{};
	int bufferHeight{};
	unsigned char const* screenBuffer{ chip8.GetScreenBuffer(bufferWidth, bufferHeight) };

	const int pixelSize{ g_WindowWidth / bufferWidth };

	for(int y{}; y<bufferHeight; ++y)
	{
		for(int x{}; x<bufferWidth; ++x)
		{
			if(screenBuffer[x + y*bufferWidth])
			{
				SDL_Rect drawRect{ x * pixelSize, y * pixelSize, pixelSize, pixelSize };
				SDL_RenderDrawRect(pRenderer, &drawRect);
			}
		}
	}
}
