#include "XXXXBackend.hpp"
#include "chip8Backend.hpp"
#include <stdint.h>


uint64_t XXXXBackend::getUptimeMs(){
	return 0;
}

uint32_t XXXXBackend::initDisplay(uint16_t width, uint16_t height, char* name){
	uint32_t ret = 0;
	return ret;
}

void XXXXBackend::closeDisplay(){

}

uint32_t XXXXBackend::initAudio(){
	uint32_t ret = 0;
	return ret;
}

uint32_t XXXXBackend::eventPoll(){
	uint32_t ret = EVENT_NONE;
	return ret;
}

uint8_t XXXXBackend::isKeyPressed(uint8_t key){
	return 0;
}

int8_t XXXXBackend::getCurrentPressedKey(){
	int ret = -1;
	return ret;
}

uint8_t XXXXBackend::getRandomNumber(){
	return 0;
}

void XXXXBackend::renderFrame(uint32_t* vram, uint8_t width, uint8_t height){

}

void XXXXBackend::playTone(){

}

void XXXXBackend::consoleWrite(uint8_t logLevel, const char* fmt, ...){

}


