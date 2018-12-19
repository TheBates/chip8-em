#include "linuxSDL2Backend.hpp"
#include "chip8Backend.hpp"
#include <SDL2/SDL.h>
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <cstdio>

#include <unistd.h>

uint64_t linuxSDL2Backend::getUptimeMs(){
	std::chrono::milliseconds uptime(0u);
	double uptimeSeconds;
	if (std::ifstream("/proc/uptime", std::ios::in) >> uptimeSeconds){
  		uptime = std::chrono::milliseconds(
    			static_cast<unsigned long long>(uptimeSeconds*1000ULL));
	}
	return uptime.count();
}

uint32_t linuxSDL2Backend::initDisplay(uint16_t width, uint16_t height, char* name){
	uint32_t ret = 0;
	SDL_Init(SDL_INIT_VIDEO);
	this->window   = SDL_CreateWindow(
      					name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
						width * 10, height * 10, SDL_WINDOW_SHOWN);
	this->renderer =  SDL_CreateRenderer(this->window, -1, 
						SDL_RENDERER_ACCELERATED);
	this->texture  = SDL_CreateTexture(this->renderer, 
						SDL_PIXELFORMAT_ARGB8888,
						SDL_TEXTUREACCESS_STATIC, 
						width, height);
	if(this->texture == NULL || this->renderer == NULL 
		|| this->texture == NULL){
			ret++;
	}
	return ret;
}

void linuxSDL2Backend::closeDisplay(){
	SDL_DestroyTexture(this->texture);
	SDL_DestroyRenderer(this->renderer);
	SDL_DestroyWindow(this->window);
	SDL_Quit();
}

uint32_t linuxSDL2Backend::initAudio(){
	return 1;
}

uint32_t linuxSDL2Backend::eventPoll(){
	uint32_t ret = EVENT_NONE;
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if(event.type == SDL_QUIT){
			this->closeDisplay();
			ret = EVENT_QUIT;
		}
	}
	return ret;
}

uint8_t linuxSDL2Backend::isKeyPressed(uint8_t key){
	/*+-+-+-+-+                +-+-+-+-+
	  |1|2|3|C|                |1|2|3|4|
	  +-+-+-+-+                +-+-+-+-+
	  |4|5|6|D|                |Q|W|E|R|
	  +-+-+-+-+       =>       +-+-+-+-+
	  |7|8|9|E|                |A|S|D|F|
	  +-+-+-+-+                +-+-+-+-+
	  |A|0|B|F|                |Z|X|C|V|
	  +-+-+-+-+                +-+-+-+-+*/
	return (SDL_GetKeyboardState(NULL)[this->keyMap[key]]);
}

int8_t linuxSDL2Backend::getCurrentPressedKey(){
	int ret = -1;

	for(int i = 0; i < KEY_MAP_SIZE; i++){
		if(SDL_GetKeyboardState(NULL)[this->keyMap[i]]){
			ret = i;
			break;
		}
	}

	return ret;
}

uint8_t linuxSDL2Backend::getRandomNumber(){
	srand(this->getUptimeMs());
	return (rand() % 255);
}

void linuxSDL2Backend::renderFrame(uint32_t* vram, uint8_t width, uint8_t height){
	SDL_UpdateTexture(this->texture, NULL, vram,
		width * sizeof(uint32_t));
	SDL_RenderCopy(this->renderer, this->texture, NULL, NULL);
	SDL_RenderPresent(this->renderer);
}

void linuxSDL2Backend::playTone(){

}

void linuxSDL2Backend::consoleWrite(uint8_t logLevel, const char* fmt, ...){
	if(logLevel <= MAX_LOG_LEVEL){
		va_list argptr;
		va_start(argptr, fmt);
		vfprintf(stdout, fmt, argptr);
		va_end(argptr);

	}
}

void linuxSDL2Backend::setKeyMap(SDL_Scancode* keyCodes){
	for(int i = 0; i < KEY_MAP_SIZE; i++){
		this->keyMap[i] = keyCodes[i];
	}
}

