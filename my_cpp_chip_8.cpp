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

void chip_8_op(char first_byte, 
			   char second_byte,
			   char *memory,
			   int *PC,
			   char *SP,
			   int *I,
			   int *registers,
			   int *stack,
			   char *display) {
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
	case 0xA:
		//Annn - LD I, addr
		//Set I = nnn.
		cout << "set index register I";
		*I = NNN;
		break;
	case 0xD:		
	{
		/*Draws a sprite at coordinate(VX, VY) that has a width of 8 pixels and a height of N pixels.Each
		row of 8 pixels is read as bit - coded starting from memory location I;
		I value does not change after the execution of this instruction.As described above,
		VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn,
		and to 0 if that does not happen*/
		//char height = N;

		//for (int i = 0; i < height; i++) {
		//	//char bit_position = *Vx % 8;
		//	//int byte_position = *Vx / 8;
		//	//// 128 = 1000 0000
		//	//char bitmask = (0b10000000) >> bit_position;
		//	//// divide windowwidth by 8 to normalize back into byte form
		//	//int index = i * (WINDOW_WIDTH / 8) + byte_position;
		//	//bool filled = (display[index] & bitmask) != 0;
		//	//int x = byte_position * 8 + bit_position;
		//	//int y = i;
		//	display[];
		//}	
		int index = (*Vy%32) * 8 + (*Vx%63);
		int height = N;
		cout << std::dec
			<< "display \nVx: "
			<< *Vx
			<< "\nVy: "
			<< *Vy
			<< "\nindex: "
			<< index
			<< "\nI: "
			<< *I
			<< "\nHeight: "
			<< height
			<< "\nDisplay B:"
			<< (bitset<8> (display[index]))
			<< "\nMem: "
			<< (bitset<8> (memory[*I]));
		for (int i = 0; i < N; i++) {
			index = (*Vy+i)%32 * 8 + *Vx % 64;
			display[index] = display[index] ^ memory[*I+i];
		}		

		break;
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
		char program[4096];		
		/*Chip - 8 has 16 general purpose 8 - bit registers, 
		usually referred to as Vx, where x is a hexadecimal digit(0 through F).
		There is also a 16 - bit register called I.This register is generally used to store memory addresses, 
		so only the lowest(rightmost) 12 bits are usually used.*/
		int PC = 512;
		char SP = 0;
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
		char display[32*8];
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
		
		if (romifilestream.read(program+512, size)) {
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
				output_display(display);
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
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