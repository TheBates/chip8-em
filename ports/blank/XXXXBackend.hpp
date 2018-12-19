#ifndef  LINUX_SDL2_BACKEND_H
#define  LINUX_SDL2_BACKEND_H

#include "chip8Backend.hpp"
#include <stdint.h>


class XXXXBackend : public chip8Backend{
        public:
	uint64_t getUptimeMs();
	uint32_t initDisplay(uint16_t width, uint16_t height, char* name);
        uint32_t initAudio();
        uint32_t eventPoll();
        uint8_t  isKeyPressed(uint8_t key);
        int8_t   getCurrentPressedKey();
        uint8_t  getRandomNumber();
        void     closeDisplay();
        void     renderFrame(uint32_t* vram, uint8_t width, uint8_t height);
        void     playTone();
        void     consoleWrite(uint8_t logLevel, const char* fmt, ...);

};

#endif
