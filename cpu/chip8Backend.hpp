#ifndef CHIP8_BACKEND_H
#define CHIP8_BACKEND_H

#include <stdint.h>

const uint32_t EVENT_NONE = 0x00;
const uint32_t EVENT_QUIT = 0x01;

const uint8_t  KEY_MAP_SIZE = 16;

const uint8_t  LOG_LEVEL_ERR = 1;
const uint8_t  LOG_LEVEL_INF = 2;
const uint8_t  LOG_LEVEL_DBG = 3;

#ifndef MAX_LOG_LEVEL
#define MAX_LOG_LEVEL (2)
#endif


class chip8Backend{
	public:
	/* Return the current program runtime in milliseconds */
	virtual uint64_t getUptimeMs()		           = 0;
	/* Setup graphics interface */
	virtual uint32_t initDisplay(uint16_t width, uint16_t height, char* name) = 0;
	/* Setup audio interface */
	virtual uint32_t initAudio()       	           = 0;
	/* Poll for exit or other window event */
	virtual uint32_t eventPoll()				   = 0;
	/* Return keypress status for keyMap[key]; 1 = pressed 0 = depressed */
	virtual uint8_t  isKeyPressed(uint8_t key)     = 0;
	/* Return first index of keyMap that is pressed, -1 if no keys are pressed */
	virtual int8_t   getCurrentPressedKey()		   = 0;
	/* Return 8-Bit random number */
	virtual uint8_t  getRandomNumber() 	           = 0;
	/* Deactivate and clean-up graphics interface */
	virtual void 	 closeDisplay() 			   = 0;
	/* Render contents of vram to texture, and blit to the display */
	virtual void	 renderFrame(uint32_t* vram, uint8_t width, uint8_t height) = 0;
	virtual void	 playTone()       	           = 0;
	virtual void	 consoleWrite(uint8_t logLevel, const char* fmt, ...)  = 0;

};

#endif
