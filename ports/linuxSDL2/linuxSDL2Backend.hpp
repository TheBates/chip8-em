#ifndef  LINUX_SDL2_BACKEND_H
#define  LINUX_SDL2_BACKEND_H

#include "chip8Backend.hpp"
#include <stdint.h>
#include <SDL2/SDL.h>

const uint32_t PIXEL_ON_ARGB_8888  = 0xFFFFFFFF;
const uint32_t PIXEL_OFF_ARGB_8888 = 0xFF000000;



class linuxSDL2Backend : public chip8Backend{
	SDL_Window*   window;
        SDL_Renderer* renderer;
        SDL_Texture*  texture;
        SDL_Scancode  keyMap[KEY_MAP_SIZE];
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
        void     setKeyMap(SDL_Scancode* keyCodes);

};

#endif
