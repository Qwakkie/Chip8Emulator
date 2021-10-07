#include <fstream>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "Chip8.h"

void Chip8::Initialize()
{
	/*
	Systems Memory Map
	0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
	0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
	0x200-0xFFF - Program ROM and work RAM
	*/

	spriteAddress = 0x050;
	instructionPointer = 0x200;
	opcode = 0;
	indexRegister = 0;
	stackPointer = 0;

	//Clear display

	
	//Clear stack
	for(int i{}; i<stackSize; ++i)
	{
		stack[i] = 0;
	}
	stackPointer = 0;
	//Clear registers
	for(int i{}; i < registerAmount; ++i)
	{
		registers[i] = 0;
	}
	//Clear memory

	for(int i{}; i<memorySize; ++i)
	{
		memory[i] = 0;
	}

	//Load fontset
	//for (int i{}; i < 80; ++i)
		//memory[i] = chip8_fontset[i];
}

void Chip8::LoadGame(const std::string& filePath)
{
	std::ifstream input{};
	input.open(filePath, std::ios::in | std::ios::binary);
	unsigned int memoryPos{};
	
	if (!input.is_open())
	{
		return;
	}
	
	char byte{};
	while(!input.eof() && memoryPos < memorySize)
	{
		input.read(&byte, sizeof(char) );
		memory[memoryPos + 512] = byte;
		++memoryPos;
	}
}

void Chip8::EmulateCycle()
{
	//Fetch opcode
	opcode = memory[instructionPointer] << 8 | memory[instructionPointer + 1];  // NOLINT(clang-diagnostic-implicit-int-conversion)

	const int x{ (opcode & 0x0F00) >> 8 };
	const int y{ (opcode & 0x00F0) >> 4 };
	
	//Decode opcode
	switch(opcode & 0xF000)
	{
	case 0x0000:
		switch(opcode & 0x000F)
		{
		case 0x000: //0x00E0: Clear the screen
			break;
		case 0x000E: //0x000E: Return from subroutine
			--stackPointer;
			instructionPointer = stack[stackPointer];
			break;
		default:
			printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
			break;
		}
		
	case 0x1000: //0x1NNN: Jump to address NNN
		instructionPointer = opcode & 0x0FFF;
		break;
		
	case 0x2000: //0x2NNN: Call subroutine at NNN
		stack[stackPointer] = instructionPointer;
		++stackPointer;
		instructionPointer = opcode & 0x0FFF;
		break;

	case 0x3000: //0x3XNN: Skip next instruction if VX == NN
		instructionPointer += 2;
		if (registers[x] == (opcode & 0x00FF))
			instructionPointer += 2;
		break;

	case 0x4000: //0x4XNN: Skip next instruction if VX == NN
		if (registers[x] != (opcode & 0x00FF))
			instructionPointer += 2;
		instructionPointer += 2;
		break;
		
	case 0x5000: //0x5XY0: Skip next instruction if VX == VY
		if (registers[x] == registers[y])
			instructionPointer += 2;
		instructionPointer += 2;
		break;

	case 0x6000: //0x6NN: Set VX to NN
		registers[x] = opcode & 0x00FF;
		instructionPointer += 2;
		break;

	case 0x7000: //0x7XNN: Add NN to VX (No carry flag)
		registers[x] += opcode & 0x00FF;
		instructionPointer += 2;
		break;

	case 0x8000:

		switch(opcode & 0x000F)
		{
		case 0x0000: //0x8XY0: Set VX to the value of VY
			registers[x] = registers[y];
			instructionPointer += 2;
			break;
			
		case 0x0001: //0x8XY1: Set VX to VX | VY
			registers[x] = registers[x] | registers[y];
			instructionPointer += 2;
			break;
			
		case 0x0002: //0x8XY2: Set VX to VX & VY
			registers[x] = registers[x] & registers[y];
			instructionPointer += 2;
			break;
			
		case 0x0003: //0x8XY3: Set VX to VX ^ VY
			registers[x] = registers[x] ^ registers[y];
			instructionPointer += 2;
			break;

		case 0x0004: //0x8XY4: Add VY to VX
			if (registers[y] > (0xFF - registers[x]))
				registers[0xF] = 1; //carry
			else
				registers[0xF] = 0;
			registers[x] += registers[y];
			instructionPointer += 2;
			break;
			
		case 0x0005: //0x8XY5: Subtract VY from VX
			registers[0xF] = 0;

			for (int mask = 0x01; mask <= 0x80; mask <<= 1) {
				if ((registers[y] & mask) > (registers[x] & mask)) {
					registers[0xF] = 1; //borrow
				}
			}

			registers[x] -= registers[y];
			instructionPointer += 2;
			break;
			
		case 0x0006: //0x8XY6: Store the least significant bit of VX in VF and shift VX 1 to the right
			registers[0xF] = registers[x] & 0x0001;
			registers[x] >>= 1;
			instructionPointer += 2;
			break;
			
		case 0x0007: //0x8XY7: Set VX to VY - VX. VF is set to 0 if there's a borrow
			registers[0xF] = 0;

			for (int mask = 0x01; mask <= 0x80; mask <<= 1) {
				if ((registers[x] & mask) > (registers[y] & mask)) {
					registers[0xF] = 1; //borrow
				}
			}
			
			registers[x] = registers[y] - registers[x];
			instructionPointer += 2;
			break;

		case 0x000E: //0x8XYE: Store the most significant bit of VX in VF and shift VX 1 to the left
			registers[0xF] = registers[x] & 0x80;
			registers[x] <<= 1;
			instructionPointer += 2;
			break;

		default:
			printf("Unknown opcode [0x8000]: 0x%X\n", opcode);
			break;
		}

	case 0x9000:
		if (registers[x] != registers[y])
			instructionPointer += 2;
		instructionPointer += 2;
		break;

	case 0xA000: //ANNN: Sets I to the address NNN
		indexRegister = static_cast<unsigned char>(opcode & 0x0FFF);
		instructionPointer += 2;
		break;

	case 0xB000: //BNNN: Jump to NNN + V0
		instructionPointer = registers[0] + (opcode & 0x0FFF);
		break;

	case 0xC000: //CNNN: Set VX to NN & a random number
		registers[x] = (rand() % 255) && (opcode & 0x00FF);
		instructionPointer += 2;
		break;

	case 0xD000: //DXYN: Draws a sprite
	{
		const unsigned short height{ static_cast<unsigned short>(opcode & 0x000F) };
		unsigned short pixel{ memory[instructionPointer] };

		registers[0xF] = 0;
		for (int row{1}; row <= height; row++)
		{
			for (int column = 0; column < 8; column++)
			{
				if ((pixel & (0x80 >> column)) != 0)
				{
					if (screenBuffer[(x + column + ((y + row) * screenWidth))] == 1)
						registers[0xF] = 1;
					screenBuffer[x + column + ((y + row) * 64)] ^= 1;
				}
			}
			pixel = memory[instructionPointer + row];
		}
	}

		drawFlag = true;
		instructionPointer += 2;
		break;

	case 0xE000:
		switch(opcode & 0x00FF)
		{
		case 0x009E: //Skip next instruction if stored key is pressed
			if (key[registers[x]])
				instructionPointer += 2;
			instructionPointer += 2;
			break;
			
		case 0x00A1: //Skip next instruction if stored key is not pressed
			if (!key[registers[x]])
				instructionPointer += 2;
			instructionPointer += 2;
			break;
			
		default:
			printf("Unknown opcode [0xE000]: 0x%X\n", opcode);
			break;
		}

	case 0xF000:
		switch(opcode & 0x00FF)
		{
		case 0x0007: //0xFX07: Set VX to delay timer value
			registers[x] = delayTimer;
			instructionPointer += 2;
			break;
			
		case 0x000A: //0xFX0A: Wait for next key press, store in VX
			//registers[x] = GetKey();
			instructionPointer += 2;
			break;

		case 0x0015: //0xFX15: Set delay timer to VX
			delayTimer = registers[x];
			instructionPointer += 2;
			break;

		case 0x0018: //0xFX18: Set sound timer to VX
			soundTimer = registers[x];
			instructionPointer += 2;
			break;

		case 0x001E: //0xFX1E: Increment VX
			indexRegister += registers[x];
			instructionPointer += 2;
			break;
			
		case 0x0029: //0xFX29: Set I to location of the sprite for the character in VX
			indexRegister = memory[spriteAddress + registers[x]];
			instructionPointer += 2;
			break;

		case 0x0033: //0xFX33: Store binary-coded decimal representation of XX
		{	
			const int number{ registers[x] & 0x00FF };
			memory[indexRegister] = static_cast<unsigned char>(number / 100);
			memory[indexRegister + 1] = (number / 10) % 10;
			memory[indexRegister + 2] = (number % 100) % 10;
		}
			instructionPointer += 2;
			break;

		case 0x0055: //0xFX55: Store V0 to VX in memory at address I
			for(int i{}; i<registerAmount; ++i)
			{
				memory[indexRegister + i] = registers[i];
			}
			instructionPointer += 2;
			break;

		case 0x0065: //0xFX65: Fill V0 to VX with memory at address I
			for (int i{}; i < registerAmount; ++i)
			{
				registers[i] = memory[indexRegister + i];
			}
			instructionPointer += 2;
			break;
		default:
			printf("Unknown opcode: 0x%X\n", opcode);
		}
		
	default:
		printf("Unknown opcode: 0x%X\n", opcode);
		break;
	}
	
	//Update timers
	if (delayTimer > 0)
		--delayTimer;

	if(soundTimer > 0)
	{
		if (soundTimer == 1)
			printf("BEEP!\n");
		--soundTimer;
	}
}

unsigned char const* Chip8::GetScreenBuffer(int& outScreenWidth, int& outScreenHeight)const
{
	outScreenWidth = screenWidth;
	outScreenHeight = screenHeight;
	return screenBuffer;
}

bool Chip8::DrawFlag()
{
	return drawFlag;
}

void Chip8::SetKeys()
{
	key[0] = static_cast<unsigned char>(GetKeyState('X'));
	key[1] = static_cast<unsigned char>(GetKeyState('1'));
	key[2] = static_cast<unsigned char>(GetKeyState('2'));
	key[3] = static_cast<unsigned char>(GetKeyState('3'));
	key[4] = static_cast<unsigned char>(GetKeyState('Q'));
	key[5] = static_cast<unsigned char>(GetKeyState('W'));
	key[6] = static_cast<unsigned char>(GetKeyState('E'));
	key[7] = static_cast<unsigned char>(GetKeyState('A'));
	key[8] = static_cast<unsigned char>(GetKeyState('S'));
	key[9] = static_cast<unsigned char>(GetKeyState('D'));
	key[10] = static_cast<unsigned char>(GetKeyState('Z'));
	key[11] = static_cast<unsigned char>(GetKeyState('C'));
	key[12] = static_cast<unsigned char>(GetKeyState('4'));
	key[13] = static_cast<unsigned char>(GetKeyState('R'));
	key[14] = static_cast<unsigned char>(GetKeyState('F'));
	key[15] = static_cast<unsigned char>(GetKeyState('V'));
}
