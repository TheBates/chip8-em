CC=
ELF:=chip8-em
CPPFLAGS= -Wall -pedantic

linuxSDL2: CC+=g++ 


linuxSDL2: CPPFLAGS+=-DMAX_LOG_LEVEL=2 -std=c++14 -lSDL2


linuxSDL2: ELF:=$(ELF)-linuxSDL2


linuxSDL2-debug: CC+=g++ 


linuxSDL2-debug: CPPFLAGS+=-DMAX_LOG_LEVEL=3 -std=c++14 -lSDL2


linuxSDL2-debug: ELF:=$(ELF)-linuxSDL2-debug


linuxSDL2: objDirectory objs/main.o objs/linuxSDL2Main.o objs/linuxSDL2Backend.o objs/chip8Core.o
	$(CC) objs/main.o objs/linuxSDL2Main.o objs/linuxSDL2Backend.o objs/chip8Core.o $(CPPFLAGS) -o $(ELF) 


linuxSDL2-debug: objDirectory objs/main.o objs/linuxSDL2Main.o objs/linuxSDL2Backend.o objs/chip8Core.o
	$(CC) objs/main.o objs/linuxSDL2Main.o objs/linuxSDL2Backend.o objs/chip8Core.o $(CPPFLAGS) -o $(ELF)

objs/main.o: main.cpp
	$(CC) -c $(CPPFLAGS) main.cpp -o objs/main.o

objs/linuxSDL2Main.o: ports/linuxSDL2/linuxSDL2Main.cpp
	$(CC) -c $(CPPFLAGS) ports/linuxSDL2/linuxSDL2Main.cpp -Icpu/ -o objs/linuxSDL2Main.o

objs/linuxSDL2Backend.o: ports/linuxSDL2/linuxSDL2Backend.cpp ports/linuxSDL2/linuxSDL2Backend.hpp cpu/chip8Backend.hpp
	$(CC) -c $(CPPFLAGS) ports/linuxSDL2/linuxSDL2Backend.cpp -Icpu/ -o objs/linuxSDL2Backend.o

objs/chip8Core.o: cpu/chip8Core.cpp cpu/chip8Core.hpp cpu/chip8Backend.hpp cpu/chip8InstructionSet.hpp cpu/chip8Font.hpp
	$(CC) -c $(CPPFLAGS) cpu/chip8Core.cpp -o objs/chip8Core.o

#objs/fileWriter.o: fileWriter.c fileWriter.h
#	$(CC) -c $(CCFLAGS) fileWriter.c -o objs/fileWriter.o

#objs/perfMonitor.o: perfMonitor.c perfMonitor.h
#	$(CC) -c $(CCFLAGS) perfMonitor.c -o objs/perfMonitor.o

objDirectory:
	mkdir -p objs/

clean:
	rm -f chip8-em-* objs/*.o
