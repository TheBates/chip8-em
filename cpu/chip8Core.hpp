#ifndef CHIP8_CORE_H
#define CHIP8_CORE_H
#include <stdint.h>

#include "chip8Backend.hpp"

const uint64_t CHIP8_60_HZ    = 16;
const uint32_t PIXEL_ON_ARGB  = 0xFFFFFFFF;
const uint32_t PIXEL_OFF_ARGB = 0xFF000000;
const uint16_t RAM_SIZE       = 0xFFF; //4095
const uint16_t FONT_START	  = 0x050;
const uint16_t RAM_START      = 0x200; //512
const uint16_t STACK_START    = 0xEA0; //3744
const uint16_t FRAME_START    = 0xF00; //3840
const uint16_t SCREEN_WIDTH   = 64;
const uint16_t SCREEN_HEIGHT  = 32;
const uint8_t  DELAY_TIMER    = 0;
const uint8_t  AUDIO_TIMER    = 1;

class chip8Core{

	/*
		- Has 4096 (0x1000) memory locations, all of which are 8 bits
		  (a byte) which is where the term CHIP-8 originated.
		- The CHIP-8 interpreter itself occupies the first 512
		  bytes of the memory space on these machines. For this reason, most
		  programs written for the original system begin at memory location
		  512 (0x200) and do not access any of the memory below the location
		  512 (0x200).
		- The uppermost 256 bytes (0xF00-0xFFF) are reserved
		  for display refresh, and the 96 bytes below that (0xEA0-0xEFF)
		  were reserved for call stack, internal use, and other variables.
		- There is no need
		  for any of the lower 512 bytes memory space to be used, but it is
		  common to store font data in those lower 512 bytes (0x000-0x200).


	*/
	uint8_t* mem;
	uint32_t* fb;
	/*
		- CHIP-8 has 16 8-bit data registers named from V0 to VF.
		- The VF register doubles as a flag for some instructions,
		  thus it should be avoided.
                - In addition operation VF is for carry flag. While in
		- subtraction, it is the "no borrow" flag. In the draw instruction
		  the VF is set upon pixel collision.
	*/
	uint8_t  V[0xF];
	/*
		- The address register, which is named I, is 16 bits wide and is
                used with several opcodes that involve memory operations.
	*/
	uint16_t I;
	uint16_t PC;
	/*
		- Delay Timer - System halts until timer reaches 0
		- Audio Timer - Tone will play until timer reaches 0
		- Timers decrement at a rate of 60hz (16.66ms)
	*/
	uint8_t  T[2];
	uint8_t drawFlag;

	uint16_t SP;
	chip8Backend& backend;

	inline void toBCD(uint8_t reg){
        	mem[I]     =  (reg / 100);
        	reg        -= (reg / 100) * 100;
        	mem[I + 1] =  (reg / 10);
        	reg        -= (reg / 10)  * 10;
        	mem[I + 2] =  (reg);
	}

	inline void chip8PushStack(){
        	if((SP + 2) < FRAME_START){
                	mem[SP]     = (PC + 2) >> 8;
                	mem[SP + 1] = (PC + 2) & 0xFF;
                	SP += 2;
        	}
	}

	inline void chip8PopStack(){
        	if(SP > STACK_START){
                	PC = (mem[SP - 2] << 8) | mem[SP - 1];
                	SP -= 2;
        	}
	}

	void chip8RegInit();
	void chip8Step();
	void chip8ServeTimers(uint64_t delta);

	public:
	chip8Core(chip8Backend& be);
	void chip8Load(uint8_t* program, uint32_t len);
	void chip8Start();
};

#endif
