#ifndef CHIP8_INSTRUCTION_SET_H
#define CHIP8_INSTRUCTION_SET_H

/* CHIP-8 OPCODES: */
/* Top Nibble == 0 */
const uint8_t  CHIP8_CLR  =  0xE0;
const uint8_t  CHIP8_RET  =  0xEE;
/*-----------------*/
const uint8_t  CHIP8_JMP  =  0x1;
const uint8_t  CHIP8_CALL =  0x2;
const uint8_t  CHIP8_JEQ  =  0x3;
const uint8_t  CHIP8_JNEQ =  0x4;
const uint8_t  CHIP8_JREG =  0x5;
const uint8_t  CHIP8_LDV  =  0x6;
const uint8_t  CHIP8_ADD  =  0x7;
/* Top Nibble == 8 */
const uint8_t  CHIP8_SET  =  0x0;
const uint8_t  CHIP8_OR   =  0x1;
const uint8_t  CHIP8_AND  =  0x2;
const uint8_t  CHIP8_XOR  =  0x3;
const uint8_t  CHIP8_ADDV =  0x4;
const uint8_t  CHIP8_SUB  =  0x5;
const uint8_t  CHIP8_RTSH =  0x6;
const uint8_t  CHIP8_SUBV =  0x7;
const uint8_t  CHIP8_LTSH =  0xE;
/*-----------------*/
const uint8_t  CHIP8_JNEV =  0x9;
const uint8_t  CHIP8_LDI  =  0xA;
const uint8_t  CHIP8_JMPO =  0xB;
const uint8_t  CHIP8_RAND =  0xC;
const uint8_t  CHIP8_DISP =  0xD;
/* Top Nibble == 15 */
const uint8_t  CHIP8_JMPK = 0x9E;
const uint8_t  CHIP8_JPNK = 0xA1;
const uint8_t  CHIP8_LVDT = 0x07;
const uint8_t  CHIP8_BLIO = 0x0A;
const uint8_t  CHIP8_LDDT = 0x15;
const uint8_t  CHIP8_LDAT = 0x18;
const uint8_t  CHIP8_ADDI = 0x1E;
const uint8_t  CHIP8_CHAR = 0x29;
const uint8_t  CHIP8_BCD  = 0x33;
const uint8_t  CHIP8_SAVV = 0x55;
const uint8_t  CHIP8_LDAV = 0x65;
/*-----------------*/

#endif
