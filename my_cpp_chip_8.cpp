#include <stdlib.h>
#include "display.hpp"
#include <SDL.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <chrono>
#include <thread>

using namespace std;

#undef main
#define WINDOW_WIDTH 600

void chip_8_op(unsigned char first_byte, 
			   unsigned char second_byte,
			   unsigned char *memory,
			   int *PC,
			   unsigned char *SP,
			   int *I,
			   int *registers,
			   int *stack,
			   unsigned char *display) {
	// The original implementation of the Chip - 8 language includes 36 different instructions, including 
	// math, graphics, and flow control functions.
	// Super Chip - 48 added an additional 10 instructions, for a total of 46.
	// All instructions are 2 bytes longand are stored most - significant - byte first.In memory, the first byte of each instruction should be located at an even addresses.If a program includes sprite data, it should be padded so any instructions following it will be properly situated in RAM.
	// This document does not yet contain descriptions of the Super Chip - 48 instructions.They are, however, listed below.
	// In these listings, the following variables are used :

	//  nnn or addr - A 12 - bit value, the lowest 12 bits of the instruction
	//	n or nibble - A 4 - bit value, the lowest 4 bits of the instruction
	//	x - A 4 - bit value, the lower 4 bits of the high byte of the instruction
	//	y - A 4 - bit value, the upper 4 bits of the low byte of the instruction
	//	kk or byte - An 8 - bit value, the lowest 8 bits of the instruction

	*PC += 2;

	int first_nibble = (first_byte & 0xf0) >> 4;
	int second_nibble = first_byte & 0x0f;
	int third_nibble = (second_byte & 0xf0) >> 4;
	int fourth_nibble = second_byte & 0x0f;
	int N = fourth_nibble;
	int NN = (third_nibble << 4) | N;
	int NNN = (second_nibble << 8) | NN;
	int *Vx = &registers[second_nibble];
	int *Vy = &registers[third_nibble];
	
	cout << std::hex << first_nibble << second_nibble << third_nibble << fourth_nibble << '\n';
	//	00E0 (clear screen)
	//	1NNN(jump)
	//	6XNN(set register VX)
	//	7XNN(add value to register VX)
	//	ANNN(set index register I)
	//	DXYN(display / draw)

	switch (first_nibble) {
	case 0:
		switch (second_nibble) {
		case 0:
			switch (third_nibble) {
			case 0xE:
				switch (fourth_nibble) {
				case 0:
					//00E0 - CLS
					//Clear the display.
					cout << "clear the display";
					break;
				case 0xE:
					cout << "returns from a subroutine";
					break;
				}
				break;
			}	
			break;
		}
		break;
	case 0x1:
		//1nnn - JP addr
		//Jump to location nnn.
		//The interpreter sets the program counter to nnn.
		cout << "jump nnn " << NNN;
		*PC = NNN;
		break;
	case 0x2:
		cout << "2NNN calls subroutine at NNN";
	case 0x3:
		cout << "3XNN Skips the next instruction if Vx == NN";
	case 0x4:
		cout << "4XNN Skips the next instruction if Vx != NN";
	case 0x5:
		cout << "5XY0 Skips the next instruction if Vx == Vy";
	case 0x6:
		//6xkk - LD Vx, byte
		//Sets VX to NN.		
		cout << "Set Vx = kk";
		*Vx = NN;
		break;
	case 0x7:
		cout << "add value to register VX";
		*Vx += NN;
		break;	
	case 0x8:
		switch (fourth_nibble) {
			case 0x0: 
				cout << "8XY0 Sets Vx to the value of Vy";
				break;
			case 0x1:
				cout << "8XY1 Sets Vx to VX or Vy";
				break;
			case 0x2:
				cout << "8XY2 Sets Vx to VX and Vy";
				break;
			case 0x3:
				cout << "8XY3 Sets Vx to VX xor Vy";
				break;
			case 0x4:
				cout << "8XY4 Sets Vx to VX + Vy sets Vf to 0 when there's a borrow else 1";
				break;
			case 0x5:
				cout << "8XY5 Sets Vx to VX - Vy, Vf is set to 0 when there's a borrow else 1";
				break;
			case 0x6:
				cout << "8XY6 stores the least significant bit of Vx in Vf and then shifts Vx to the right by 1";
				break;
			case 0x7:
				cout << "8XY7 Sets Vx to VX - Vy, Vf is set to 0 when there's a borrow else 1? looks like 0x5";
				break;
			case 0xE:
				cout << "8XYE Stores the most significant bit of Vx in Vf then shifts Vx to the left by 1";
				break;
		}	
	case 0x9:
		cout << "9XY0 Skips the next instruction if Vx != Vy";
	case 0xA:
		//Annn - LD I, addr
		//Set I = nnn.
		cout << "set index register I";
		*I = NNN;
		break;
	case 0xB:
		cout << "BNNN Jumps to the address NNN plus V0";
	case 0xC:
		cout << "CXNN Sets Vx to the result of a bitwise of NN on a random number";
	case 0xD:		
	{
		/*Draws a sprite at coordinate(VX, VY) that has a width of 8 pixels and a height of N pixels.Each
		row of 8 pixels is read as bit - coded starting from memory location I;
		I value does not change after the execution of this instruction.As described above,
		VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn,
		and to 0 if that does not happen*/
		//unsigned char height = N;

		//for (int i = 0; i < height; i++) {
		//	//unsigned char bit_position = *Vx % 8;
		//	//int byte_position = *Vx / 8;
		//	//// 128 = 1000 0000
		//	//unsigned char bitmask = (0b10000000) >> bit_position;
		//	//// divide windowwidth by 8 to normalize back into byte form
		//	//int index = i * (WINDOW_WIDTH / 8) + byte_position;
		//	//bool filled = (display[index] & bitmask) != 0;
		//	//int x = byte_position * 8 + bit_position;
		//	//int y = i;
		//	display[];
		//}	
		int height = N;
		int x = *Vx;
		int y = *Vy;
		cout << "display";
		for (int i = 0; i < N; i++) {
			int byte_position = (y+i)*8 + x/8;
			int bit_offset = x % 8;
			unsigned char byte_bitmask = 0;
			if (bit_offset != 0) {				
				unsigned unsigned char left_byte = memory[*I + i];				
				left_byte = left_byte >> bit_offset;
				display[byte_position] ^= left_byte;
				unsigned unsigned char right_byte = memory[*I + i];
				right_byte = right_byte << 8-bit_offset;				
				display[byte_position + 1] ^= right_byte;
			}
			else 
				display[byte_position] ^= memory[*I + i];
			output_display(display);
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}		
		break;
	}	
	case 0xE: {
		switch (third_nibble) {
		case 0x9:
			cout << "EX9E Skips the next instruction if the key stored in Vx is pressed";
			break;
		case 0xA:
			cout << "EX9E Skips the next instruction if the key stored in Vx is not pressed";
			break;
		}
	}
	case 0xF: {
		switch (third_nibble) {
		case 0x0:
			switch (fourth_nibble) {
			case 0x7:
				cout << "Sets Vx to the value of the delay timer.";
				break;
			case 0xA:
				cout << "Sets Vx to a key press (blocking operationg, all instruction halted until next key)";
				break;
			}
			break;
		case 0x1:
			switch (fourth_nibble) {
			case 0x5:
				cout << "Sets the delay timer to Vx";
				break;
			case 0x8:
				cout << "Sets the sound timer to Vx";
				break;
			case 0xE:
				cout << "Sets I to I + Vx, Vf is not affected";
				break;
			}
			break;
		case 0x2:
			cout << "FX29 Sets I to the location of the sprite for the character in Vx";
			break;
		case 0x3:
			/*set_BCD(Vx)
				* (I + 0) = BCD(3);
			*(I + 1) = BCD(2);
			*(I + 2) = BCD(1);*/
			cout << "FX33 Stores the binary-coded decimal reprentation of Vx with the most significant of three digits at the address of I, the middle digit at I plus 1, and the least significant digit at I plus 2.";
			break;
		case 0x5:
			cout << "FX55 Stores from V0 to Vx in memory starting at address I, the offset of I is increased by 1 for each value written, but I itself is left unmodified";
			break;
		case 0x6:
			cout << "FX65 Fills from V0 to Vx with values from memory, starting at address I. The offset from I is increased by 1 for each value written.";
			break;
		}
	}
	default:
		cout << "unknown instruction";
	}
	cout << '\n\n';
}

int main(int argc, char *argv[]) {    
	ifstream romifilestream;
	romifilestream.open(argv[1], ios::in | ios::binary | ios::ate);
	if (romifilestream) {
		cout << argv[1] << " successfully opened";
		//The Chip - 8 language is capable of accessing up to 4KB(4, 096 bytes) of RAM, from location 0x000 (0) to 0xFFF (4095).
		//The first 512 bytes, from 0x000 to 0x1FF, are where the original interpreter was located, and should not be used by programs.
		//	Most Chip - 8 programs start at location 0x200 (512), but some begin at 0x600 (1536).Programs beginning at 0x600 are intended for the ETI 660 computer.
		//	Memory Map :
		//+-------------- - += 0xFFF (4095) End of Chip - 8 RAM
		//	|               |
		//	|               |
		//	|               |
		//	|               |
		//	|               |
		//	| 0x200 to 0xFFF|
		//	|     Chip - 8    |
		//	| Program / Data|
		//	|     Space     |
		//	|               |
		//	|               |
		//	|               |
		//	+--------+= 0x600 (1536) Start of ETI 660 Chip - 8 programs
		//	|               |
		//	|               |
		//	|               |
		//	+-------------- - += 0x200 (512) Start of most Chip - 8 programs
		//	| 0x000 to 0x1FF|
		//	| Reserved for  |
		//	|  interpreter |
		//	+-------------- - += 0x000 (0) Start of Chip - 8 RAM
		unsigned char program[4096];		
		/*Chip - 8 has 16 general purpose 8 - bit registers, 
		usually referred to as Vx, where x is a hexadecimal digit(0 through F).
		There is also a 16 - bit register called I.This register is generally used to store memory addresses, 
		so only the lowest(rightmost) 12 bits are usually used.*/
		int PC = 512;
		unsigned char SP = 0;
		/*Chip - 8 has 16 general purpose 8 - bit registers, usually referred to as Vx, 
		where x is a hexadecimal digit(0 through F).There is also a 16 - bit register called I.
		This register is generally used to store memory addresses, so only the lowest(rightmost) 12 bits are usually used.*/
		int registers[16];
		/* The stack is an array of 16 16 - bit values, 
		used to store the address that the interpreter shoud return to when finished with a subroutine.Chip - 
		8 allows for up to 16 levels of nested subroutines.*/
		int stack[16];
		/*The original implementation of the Chip - 8 language used a 64x32 - pixel monochrome display with this format:
		(0, 0)	(63, 0)
		(0, 31)	(63, 31)*/
		unsigned char display[32*8];
		//I : 16bit register (For memory address) (Similar to void pointer);
		int I = 0;
		for (int i = 0; i < (32 * 8); i++) {
			display[i] = 0;
		}
		// font setup
		//0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		//0x20, 0x60, 0x20, 0x20, 0x70, // 1
		//0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		//0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		//0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		//0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		//0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		//0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		//0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		//0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		//0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		//0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		//0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		//0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		//0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		//0xF0, 0x80, 0xF0, 0x80, 0x80  // F

		streamsize size = romifilestream.tellg();
		romifilestream.seekg(0, ios::beg);
		
		if (romifilestream.read((char*)program+512, size)) {
			cout << " successfully read chip-8 program\n";
			init();
			for (int i = 0; i < size/2; i++) {
				chip_8_op(program[PC], 
						  program[PC+1],
						  program,
						  &PC,
						  &SP,
						  &I,
						  registers,
						  stack,
						  display);				
			}						
		} 
		else {
			cout << " failed to read chip-8 program";
		}
		while (1) {
			//if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
			//	break;
		}
		//SDL_DestroyRenderer(renderer);
		//SDL_DestroyWindow(window);
		//SDL_Quit();
		romifilestream.close();
	}
	else { 
		cout << argv[1] << " file failed to open";
	}	
	return 0;
}