all:
	g++ src/custom/src/*.cpp src/custom/asm.cpp src/custom/memmgr.cpp -ggdb3 -o game
	g++ 2.cpp -I/usr/include/SDL2 -D_REENTRANT -pthread -lSDL2 -ggdb3 -o sdl_debug
