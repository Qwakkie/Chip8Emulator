#pragma once
#include <string>

class Chip8
{
public:
	void Initialize();
	void LoadGame(const std::string&);
	void EmulateCycle();
	unsigned char const* GetScreenBuffer(int& outScreenWidth, int& outScreenHeight)const;
	bool DrawFlag();
	void SetKeys();
private:
	//35 opcodes, 2 bytes long
	unsigned short opcode{};

	//4K memory
	static const int memorySize{ 4096 };
	unsigned char memory[memorySize]{};

	//15 8-bit registers V0-VE + 1 carry flag
	static const int registerAmount{ 16 };
	unsigned char registers[registerAmount]{};

	unsigned char indexRegister{};

	unsigned short spriteAddress{};
	unsigned short instructionPointer{};

	static const int screenWidth{ 64 };
	static const int screenHeight{ 32 };
	unsigned char screenBuffer[screenWidth * screenHeight]{};

	unsigned char delayTimer{};
	unsigned char soundTimer{};

	static const int stackSize{ 16 };
	unsigned short stack[stackSize]{};
	unsigned short stackPointer{};

	static const int keyAmount{ 16 };
	unsigned char key[keyAmount]{};

	bool drawFlag{true};
};

