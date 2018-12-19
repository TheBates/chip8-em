#include "chip8Core.hpp"
#include "chip8InstructionSet.hpp"
#include "chip8Backend.hpp"
#include "chip8Font.hpp"
#include <cstring>
#include <cstdio>
#include <unistd.h>
/*http://www.multigesture.net/wp-content/uploads/mirror/goldroad/chip8.shtml*/

void chip8Core::chip8RegInit(){
    mem = new uint8_t[RAM_SIZE];
	fb  = new uint32_t[SCREEN_WIDTH * SCREEN_HEIGHT];
        memset(mem, 0, RAM_SIZE);
	memset(fb,  0, (SCREEN_WIDTH * SCREEN_HEIGHT) * sizeof(uint32_t));
        memset(V, 0, 0xF);
	memcpy(&mem[FONT_START], font4x5, FONT_4X5_BYTE_LEN);
        PC = I = RAM_START;
	SP = STACK_START;
	T[DELAY_TIMER] = T[AUDIO_TIMER] = 0;
	drawFlag = 0;
}

void chip8Core::chip8Step(){
	uint16_t opcode       = ((mem[PC] << 8) + (mem[PC+1]));
	uint8_t  bottomByte   = (opcode & 0x00FF);
	uint8_t  topByte      = ((opcode & 0xFF00) >> 8);
	uint8_t  nibbles[4];
	nibbles[0] = ((bottomByte & 0x0F));
	nibbles[1] = ((bottomByte & 0xF0) >> 4);
	nibbles[2] = ((topByte    & 0x0F));
	nibbles[3] = ((topByte 	  & 0xF0) >> 4);
	int8_t curKey;
	bool incPC  = true;

	/*printf("opcode     is 0x%x\n", opcode);
	printf("bottomByte is 0x%x\n", bottomByte);
	printf("topByte    is 0x%x\n", topByte);

	for(int j = 0; j < 4; j++){
		printf("nibbles[%d] is 0x%x\n", j, nibbles[j]);
	}*/
	if(!nibbles[3]){
		/*
		CHIP8_CLR:

		Desc:
			Clears the screen.
		Bit Pattern:
			00E0
		Args:
			None.
		*/
		if(bottomByte == CHIP8_CLR){
			backend.consoleWrite(LOG_LEVEL_DBG, "Executing: CHIP8_CLR\n");
			memset(&mem[FRAME_START], 0, (RAM_SIZE - FRAME_START));
			for(int i = 0; i < (SCREEN_WIDTH * SCREEN_HEIGHT); i++){
				fb[i] = PIXEL_OFF_ARGB;
			}
			//drawFlag = 1;
        }
		/*
		CHIP8_RET:

		Desc:
			Returns from a subroutine.
		Bit Pattern:
			00EE
		Args:
			None.
		*/
        else if(bottomByte == CHIP8_RET){
			backend.consoleWrite(LOG_LEVEL_DBG,"Executing: CHIP8_RET\n");
			chip8PopStack();
			incPC = false;
        }
        else{
			backend.consoleWrite(LOG_LEVEL_ERR, "Executing: UNKNOWN OPCODE: 0x%x \n", opcode);
        }
	}
    else if(nibbles[3] < 0x8){
        switch(nibbles[3]){
		/*
		CHIP8_JMP:

		Desc:
			Jumps to address NNN.
		Bit Pattern:
			1NNN
		Args:
			NNN - Address
		*/
        case CHIP8_JMP:
		backend.consoleWrite(LOG_LEVEL_DBG,"Executing: CHIP8_JMP\n");
		PC = ((nibbles[2] << 8) | bottomByte);
		incPC = false;
        break;
		/*
		CHIP8_CALL:

		Desc:
			Calls subroutine at NNN. Pushes PC + 2 to call stack.
		Nibble Pattern:
			2NNN
		Args:
			NNN - Address
		*/
        case CHIP8_CALL:
		backend.consoleWrite(LOG_LEVEL_DBG, "Executing: CHIP8_CALL\n");
		chip8PushStack();
		PC = ((nibbles[2] << 8) | bottomByte);
		incPC = false;
        break;
		/*
		CHIP8_JEQ:

		Desc:
			Skips the next instruction if VX equals NN. 
			(Usually the next instruction is a jump to skip a 
			code block)
		Nibble Pattern:
			3XNN
		Args:
			X - V Register to compare with NN
			NN - Value to compare with V[X]
		*/
        case CHIP8_JEQ:
		backend.consoleWrite(LOG_LEVEL_DBG, "Executing: CHIP8_JEQ\n");
		if(V[nibbles[2]] == bottomByte){
			PC += 2;
		}
        break;
		/*
		CHIP8_JNEQ:

		Desc:
			Skips the next instruction if VX doesn't equal NN. 
			(Usually the next instruction is a jump to skip 
			a code block) 
		Nibble Pattern:
			4XNN
		Args:
			X - V Register to compare with NN
			NN - Value to compare with V[X]
		*/
        case CHIP8_JNEQ:
		backend.consoleWrite(LOG_LEVEL_DBG, "Executing: CHIP8_JNEQ\n");
		if(V[nibbles[2]] != bottomByte){
        	PC += 2;
        }
        break;
		/*
		CHIP8_JREG:

		Desc:
			Skips the next instruction if VX equals VY. 
			(Usually the next instruction is a jump to skip a 
			code block) 
		Nibble Pattern:
			5XY0
		Args:
			X - V Register to compare with V[Y]
			Y - V Register to compare with V[X]
		*/
        case CHIP8_JREG:
		backend.consoleWrite(LOG_LEVEL_DBG, "Executing: CHIP8_JREG\n");
		if(V[nibbles[2]] == V[nibbles[1]]){
            PC += 2;
        }
        break;
		/*
		CHIP8_LDV:

		Desc:
			Sets VX to NN.
		Nibble Pattern:
			6XNN
		Args:
			X - V Register to assign NN to
			NN - Value to assign V[X] with
		*/
        case CHIP8_LDV:
		backend.consoleWrite(LOG_LEVEL_DBG, "Executing: CHIP8_LDV\n");
		V[nibbles[2]] = bottomByte;
        break;
		/*
		CHIP8_ADD:

		Desc:
			Adds NN to VX. (Carry flag is not changed) 
		Nibble Pattern:
			7XNN
		Args:
			X - V Register to assign NN to
			NN - Value to assign V[X] with
		*/
        case CHIP8_ADD:
		backend.consoleWrite(LOG_LEVEL_DBG, "Executing: CHIP8_ADD\n");
		V[nibbles[2]] += bottomByte;
		break;
		default:
		backend.consoleWrite(LOG_LEVEL_ERR, "Executing: UNKNOWN OPCODE: 0x%x\n", opcode);
		break;
        }
	}
	else if(nibbles[3] == 0x8){
		switch(nibbles[0]){
		/*
		CHIP8_SET:

		Desc:
			Sets VX to the value of VY.
		Nibble Pattern:
			8XY0
		Args:
			X - V Register to assign V[Y] to
			Y - V Register containing the value to assign V[X] with
		*/
		case CHIP8_SET:
		backend.consoleWrite(LOG_LEVEL_DBG, "Executing: CHIP8_SET\n");
		V[nibbles[2]] = V[nibbles[1]];
		break;
		/*
		CHIP8_OR:

		Desc:
			Sets VX to VX or VY. (Bitwise OR operation) 
		Nibble Pattern:
			8XY1
		Args:
			X - V Register to assign V[X] OR V[Y] to
			Y - V Register containing the value to OR with V[X]
		*/
		case CHIP8_OR:
		backend.consoleWrite(LOG_LEVEL_DBG, "Executing: CHIP8_OR\n");
		V[nibbles[2]] |= V[nibbles[1]];
		break;
		/*
		CHIP8_AND:

		Desc:
			Sets VX to VX and VY. (Bitwise AND operation)
		Nibble Pattern:
			8XY2
		Args:
			X - V Register to assign V[X] AND V[Y] to
			Y - V Register containing the value to AND with V[X]
		*/
		case CHIP8_AND:
		backend.consoleWrite(LOG_LEVEL_DBG, "Executing: CHIP8_AND\n");
		V[nibbles[2]] &= V[nibbles[1]];
		break;
		/*
		CHIP8_XOR:

		Desc:
			Sets VX to VX xor VY.
		Nibble Pattern:
			8XY3
		Args:
			X - V Register to assign V[X] XOR V[Y] to
			Y - V Register containing the value to XOR with V[X]
		*/
		case CHIP8_XOR:
		backend.consoleWrite(LOG_LEVEL_DBG, "Executing: CHIP8_XOR\n");
		V[nibbles[2]] ^= V[nibbles[1]];
		break;
		/*
		CHIP8_ADDV:

		Desc:
			Adds VY to VX. VF is set to 1 when there's a carry, 
			and to 0 when there isn't.
		Nibble Pattern:
			8XY4
		Args:
			X - V Register to assign V[X] + V[Y] to
			Y - V Register containing the value to add with V[X]
		*/
		case CHIP8_ADDV:
		backend.consoleWrite(LOG_LEVEL_DBG, "Executing: CHIP8_ADDV\n");
		if(((uint16_t) V[nibbles[2]] +
		    (uint16_t) V[nibbles[1]]) > 255){
			V[0xF] = 1;
		}
		else{
			V[0xF] = 0;
		}
		V[nibbles[2]] += V[nibbles[1]];
		break;
		/*
		CHIP8_SUB:

		Desc:
			VY is subtracted from VX. VF is set to 0 when there's a 
			borrow, and 1 when there isn't. 
		Nibble Pattern:
			8XY5
		Args:
			X - V Register to assign V[X] - V[Y] to
			Y - V Register containing the value to subtract with 
				V[X]
		*/
		case CHIP8_SUB:
		backend.consoleWrite(LOG_LEVEL_DBG, "Executing: CHIP8_SUB\n");
		if(V[nibbles[2]] < V[nibbles[1]]){
			V[0xF] = 1;
		}
		else{
			V[0xF] = 0;
		}
		V[nibbles[2]] -= V[nibbles[1]];
		break;
		/*
		CHIP8_RTSH:

		Desc:
			Stores the least significant bit of VX in VF and then 
			shifts VX to the right by 1.
		Nibble Pattern:
			8XY6
		Args:
			X - V Register to shift right by 1
		*/
		case CHIP8_RTSH:
		backend.consoleWrite(LOG_LEVEL_DBG, "Executing: CHIP8_RTSH\n");
		V[0xF] = V[nibbles[2]] & 0x1;
		V[nibbles[2]] >>= 1;
		break;
		/*
		CHIP8_SUBV:

		Desc:
			Sets VX to VY minus VX. VF is set to 0 when there's a 
			borrow, and 1 when there isn't. 
		Nibble Pattern:
			8XY7
		Args:
			X - V Register to assign the result of V[Y] - V[X] to
			Y - V Register to be subtracted by V[X]
		*/
		case CHIP8_SUBV:
		backend.consoleWrite(LOG_LEVEL_DBG, "Executing: CHIP8_SUBV\n");
		if(V[nibbles[1]] > V[nibbles[2]]){
                        V[0xF] = 1;
                }
                else{
                        V[0xF] = 0;
                }
		V[nibbles[2]] = V[nibbles[1]] - V[nibbles[2]];
		break;
		/*
		CHIP8_LTSH:

		Desc:
			Stores the most significant bit of VX in VF and 
			then shifts VX to the left by 1
		Nibble Pattern:
			8XYE
		Args:
			X - V Register to shift left by 1
		*/
		case CHIP8_LTSH:
		backend.consoleWrite(LOG_LEVEL_DBG, "Executing: CHIP8_LTSH\n");
		V[0xF] = V[nibbles[2]] & 0x8;
                V[nibbles[2]] <<= 1;
		break;
		default:
		backend.consoleWrite(LOG_LEVEL_ERR, "Executing: UNKNOWN OPCODE 0x%x\n", opcode);
		break;
		}
	}
	else if(nibbles[3] < 0xE){
		switch(nibbles[3]){
		/*
		CHIP8_JNEV:

		Desc:
			Skips the next instruction if VX doesn't equal VY. 
			(Usually the next instruction is a jump to skip a 
			code block) 
		Nibble Pattern:
			9XY0
		Args:
			X - V Register to compare with V[Y]
			Y - V Register to compare with V[X]
		*/
		case CHIP8_JNEV:
		backend.consoleWrite(LOG_LEVEL_DBG, "Executing: CHIP8_JNEV\n");
		if(V[nibbles[2]] != V[nibbles[1]]){
			PC += 2;
		}
		break;
		/*
		CHIP8_LDI:

		Desc:
			Sets I to the address NNN.
		Nibble Pattern:
			ANNN
		Args:
			NNN - Address to load into index register
		*/
		case CHIP8_LDI:
		backend.consoleWrite(LOG_LEVEL_DBG, "Executing: CHIP8_LDI\n");
		I = ((nibbles[2] << 8) | bottomByte);
		break;
		/*
		CHIP8_JMPO:

		Desc:
			Jumps to the address NNN plus V0.
		Nibble Pattern:
			BNNN
		Args:
			NNN - Address to load into index register
		*/
		case CHIP8_JMPO:
		backend.consoleWrite(LOG_LEVEL_DBG, "Executing: CHIP8_JMPO\n");
		PC = ((nibbles[2] << 8) | bottomByte) + V[0];
		incPC = false;
		break;
		/*
		CHIP8_RAND:

		Desc:
			Sets VX to the result of a bitwise and operation on a 
			random number (Typically: 0 to 255) and NN. 
		Nibble Pattern:
			CXNN
		Args:
			X - V Register to store resulting random number into
			NN - Bitmask for resulting random number
		*/
		case CHIP8_RAND:
		backend.consoleWrite(LOG_LEVEL_DBG, "Executing: CHIP8_RAND\n");
		V[nibbles[2]] = backend.getRandomNumber() & bottomByte;
		break;
		/*
		CHIP8_DISP:

		Desc:
			Draws a sprite at coordinate (VX, VY) that has a width 
			of 8 pixels and a height of N pixels. Each row of 8 
			pixels is read as bit-coded starting from memory 
			location I; I value doesn’t change after the execution 
			of this instruction. As described above, VF is set to 1 
			if any screen pixels are flipped from set to unset when 
			the sprite is drawn, and to 0 if that doesn’t happen.

		Nibble Pattern:
			DYXN
		Args:
			Y - V Register containing the origin Y coordinate
			X - V Register containing the origin X coordinate
			N - V Register containing the maximum height
		*/
		case CHIP8_DISP:
		backend.consoleWrite(LOG_LEVEL_DBG, "Executing: CHIP8_DISP\n");
		V[0xF] = 0;
		for(uint32_t y = 0;  y < nibbles[0]; y++){
			uint8_t pixel = mem[I + y];
            for(uint32_t x = 0; x < 8; x++){
            	if(pixel & (0x80 >> x)){
                	uint32_t index = ((V[nibbles[2]] + x) % SCREEN_WIDTH +
                    	((V[nibbles[1]] + y) % SCREEN_HEIGHT) * SCREEN_WIDTH);					
					if(fb[index] == PIXEL_ON_ARGB){
                    	V[0xF] = 1;
                        fb[index] = PIXEL_OFF_ARGB;
                    }
                    else{
                    	fb[index] = PIXEL_ON_ARGB;
                    }
                }
            }
        }
		drawFlag = 1;
		break;
		default:
		backend.consoleWrite(LOG_LEVEL_ERR, "Executing: UNKNOWN OPCODE 0x%x\n", opcode);
		break;
		}
	}
	else{
		switch(bottomByte){
		/*
		CHIP8_JMPK:

		Desc:
			Skips the next instruction if the key stored in VX is 
			pressed. (Usually the next instruction is a jump to 
			skip a code block)
		Nibble Pattern:
			EX9E
		Args:
			X - V Register containing the desired keymap index
		*/
		case CHIP8_JMPK:
		backend.consoleWrite(LOG_LEVEL_DBG, "Executing: CHIP8_JMPK\n");
		if(backend.isKeyPressed(V[nibbles[2]])){
			PC += 2;
		}
		break;
		/*
		CHIP8_JPNK:

		Desc:
			Skips the next instruction if the key stored in VX 
			isn't pressed. (Usually the next instruction is a jump 
			to skip a code block) 
		Nibble Pattern:
			EXA1
		Args:
			X - V Register containing the desired keymap index
		*/
		case CHIP8_JPNK:
		backend.consoleWrite(LOG_LEVEL_DBG, "Executing: CHIP8_JPNK\n");
		if(!backend.isKeyPressed(V[nibbles[2]])){
			PC += 2;
		}
		break;
		/*
		CHIP8_LVDT:

		Desc:
			Sets VX to the value of the delay timer.
		Nibble Pattern:
			FX07
		Args:
			X - V Register to store the delay timer value into
		*/
		case CHIP8_LVDT:
		backend.consoleWrite(LOG_LEVEL_DBG, "Executing: CHIP8_LVDT\n");
		V[nibbles[2]] = T[DELAY_TIMER];
		break;
		/*
		CHIP8_BLIO:

		Desc:
			A key press is awaited, and then stored in VX. 
			(Blocking Operation. All instruction halted until 
			next key event).
		Nibble Pattern:
			FX0A
		Args:
			X - V Register to store the current pressed key index
				into 
		*/
		case CHIP8_BLIO:
		backend.consoleWrite(LOG_LEVEL_DBG, "Executing: CHIP8_BLIO\n");
		curKey = backend.getCurrentPressedKey();
		if(curKey < 0){
			incPC = false;
		}
		else{
			V[nibbles[2]] = curKey;
		}
		break;
		/*
		CHIP8_LDDT:

		Desc:
			Sets the delay timer to VX.
		Nibble Pattern:
			FX15
		Args:
			X - V Register containing the value to store into the
				delay timer
		*/
		case CHIP8_LDDT:
		backend.consoleWrite(LOG_LEVEL_DBG, "Executing: CHIP8_LDDT\n");
		T[DELAY_TIMER] = V[nibbles[2]];
		break;
		/*
		CHIP8_LDAT:

		Desc:
			Sets the sound timer to VX.
		Nibble Pattern:
			FX18
		Args:
			X - V Register containing the value to store into the
				audio timer
		*/
		case CHIP8_LDAT:
		backend.consoleWrite(LOG_LEVEL_DBG, "Executing: CHIP8_LDAT\n");
		T[AUDIO_TIMER] = V[nibbles[2]];
		break;
		/*
		CHIP8_ADDI:

		Desc:
			Adds VX to I. VF is set to 1 when there is a range 
			overflow (I+VX>0xFFF), and to 0 when there isn't. This 
			is an undocumented feature of the CHIP-8 and used by 
			the Spacefight 2091! game.
		Nibble Pattern:
			FX1E
		Args:
			X - V Register containing the value to store into the
				audio timer
		*/
		case CHIP8_ADDI:
		backend.consoleWrite(LOG_LEVEL_DBG, "Executing: CHIP8_ADDI\n");
		if(I + V[nibbles[2] > 0xFFF]){
			V[0xF] = 1;
		}
		else{
			V[0xF] = 0;
		}
		I += V[nibbles[2]];
		break;
		/*
		CHIP8_CHAR:

		Desc:
			Sets I to the location of the sprite for the character 
			in VX. Characters 0-F (in hexadecimal) are represented 
			by a 4x5 font. 
		Nibble Pattern:
			FX29
		Args:
			X - V Register containing the digit to draw as a 
			character
		*/
		case CHIP8_CHAR:
		backend.consoleWrite(LOG_LEVEL_DBG, "Executing: CHIP8_CHAR: 0x%x\n", nibbles[2]);
		I = FONT_START + (V[nibbles[2]] * 5);
		break;
		/*
		CHIP8_BCD:

		Desc:
			Stores the binary-coded decimal representation of VX, 
			with the most significant of three digits at the address
			in I, the middle digit at I plus 1, and the least significant 
			digit at I plus 2. (In other words, take the decimal 
			representation of VX, place the hundreds digit in memory 
			at location in I, the tens digit at location I+1, and 
			the ones digit at location I+2.)  
		Nibble Pattern:
			FX33
		Args:
			X - V Register containing the value to convert into BCD
		*/
		case CHIP8_BCD:
		backend.consoleWrite(LOG_LEVEL_DBG,"Executing: CHIP8_BCD\n");
		toBCD(V[nibbles[2]]);
		break;
		/*
		CHIP8_SAVV:

		Desc:
			Stores V0 to VX (including VX) in memory starting at 
			address I. The offset from I is increased by 1 for each 
			value written, but I itself is left unmodified. 
		Nibble Pattern:
			FX55
		Args:
			X - Number of V registers to save 
			character
		*/
		case CHIP8_SAVV:
		backend.consoleWrite(LOG_LEVEL_DBG, "Executing: CHIP8_SAVV\n");
		for(int offset = 0; offset < nibbles[2]; offset++){
			mem[I + offset] = V[offset];
		}
		break;
		/*
		CHIP8_LDAV:

		Desc:
			Fills V0 to VX (including VX) with values from memory 
			starting at address I. The offset from I is increased 
			by 1 for each value written, but I itself is left 
			unmodified. 
		Nibble Pattern:
			FX55
		Args:
			X - Number of V registers to load into
		*/
		case CHIP8_LDAV:
		backend.consoleWrite(LOG_LEVEL_DBG, "Executing: CHIP8_LDAV, X = 0x%x\n", nibbles[2]);
		for(int offset = 0; offset < nibbles[2]; offset++){
            V[offset] = mem[I + offset];
        }
		break;
		default:
		backend.consoleWrite(LOG_LEVEL_ERR, "Executing: UNKNOWN OPCODE 0x%x\n", opcode);
		break;
		}
	}
	if(incPC){
        PC += 2;
	}
}

void chip8Core::chip8ServeTimers(uint64_t delta){
	uint32_t timesToServe = (delta % CHIP8_60_HZ);
	T[DELAY_TIMER] = 
		(T[DELAY_TIMER] > timesToServe) ? 
		(T[DELAY_TIMER] - timesToServe) : 0;
	T[AUDIO_TIMER] = 
		(T[AUDIO_TIMER] > timesToServe) ? 
		(T[AUDIO_TIMER] - timesToServe) : 0;

}

chip8Core::chip8Core(chip8Backend& be) : backend(be){
	this->chip8RegInit();
}

void chip8Core::chip8Load(uint8_t* program, uint32_t len){
	backend.consoleWrite(LOG_LEVEL_INF, "Writing in %d bytes to RAM...\n", len);
	if(len < RAM_SIZE - RAM_START){
		for(uint32_t i = 0; i < len; i++){
			this->mem[i+RAM_START] = program[i];
		}
	}
}

void chip8Core::chip8Start(){
	uint64_t currentTimestamp = 0;
	uint64_t lastTimestamp    = 0;
	uint64_t timeDelta;
	uint32_t event = EVENT_NONE;
	backend.initDisplay(SCREEN_WIDTH, SCREEN_HEIGHT, (char*) "chip8-em");
	backend.renderFrame(fb, SCREEN_WIDTH, SCREEN_HEIGHT);
	while(event != EVENT_QUIT){

		event = backend.eventPoll();
		currentTimestamp = backend.getUptimeMs();
		timeDelta = (currentTimestamp - lastTimestamp);

		this->chip8Step();
		if(timeDelta >= CHIP8_60_HZ){
			this->chip8ServeTimers(timeDelta);
			lastTimestamp = currentTimestamp;
		}
		if(drawFlag){
			backend.renderFrame(fb, 
				SCREEN_WIDTH, SCREEN_HEIGHT);
			drawFlag = 0;
		}
		//usleep(50000);
		//usleep(25000);
		usleep(10000);
	}
}
