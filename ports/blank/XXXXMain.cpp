#include "chip8Core.hpp"
#include "XXXXBackend.hpp"

int chip8Main(int argc, char** argv){
    int ret = 0;
    XXXXBackend backend;
	chip8Core core(backend);
    
    return ret;
}