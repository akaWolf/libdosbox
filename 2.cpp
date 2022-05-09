#include <SDL2/SDL.h>
const int SCREEN_WIDTH = 344;
const int SCREEN_HEIGHT = 240;//760;
uint32_t tempDrawBuffer[SCREEN_WIDTH*SCREEN_HEIGHT];
struct myDrawInfoS
{
  uint8_t drawBuffer[65536*4];
  SDL_Color drawPalette[256];
  uint32_t myOffset;
  uint8_t myPixelOffset;
};
struct myDrawInfoS* myDrawInfo;
SDL_Window* myWindow = NULL;
SDL_Renderer* myRenderer = NULL;
SDL_Texture* myTexture = NULL;
SDL_PixelFormat *myFormat = NULL;
unsigned int plane4_to_linear(unsigned int plane, unsigned int offset)
{
  return offset * 4 + plane;
}
uint32_t planar_to_linear(uint32_t x, uint32_t y)
{
  return (y * SCREEN_WIDTH + x);
}
void drawPixel(uint32_t offset, uint8_t color)
{
  myDrawInfo->drawBuffer[offset] = color;
}
void drawPixel(uint32_t x, uint32_t y, uint8_t color)
{
  int offset = planar_to_linear(x, y);

  myDrawInfo->drawBuffer[offset] = color;
}
void setPalette(uint8_t color, uint8_t r, uint8_t g, uint8_t b)
{
  myDrawInfo->drawPalette[color] = {r, g, b, 255};
}
void updateDraw()
 {
  for (int i = 0; i < 176 * SCREEN_WIDTH; i++)
  {
	//myDrawInfo->myOffset=0x5be8;
	//myDrawInfo->myOffset=0xa1c8;
	//myDrawInfo->myOffset=0x66a8;
	//myDrawInfo->myOffset=0;
	auto color = myDrawInfo->drawBuffer[myDrawInfo->myOffset * 4 + myDrawInfo->myPixelOffset + i];
	auto sdl_color = myDrawInfo->drawPalette[color];
	tempDrawBuffer[i + 0 * SCREEN_WIDTH] = SDL_MapRGBA(myFormat, sdl_color.r, sdl_color.g, sdl_color.b, sdl_color.a);
  }
  for (int i = 0; i < SCREEN_WIDTH * (SCREEN_HEIGHT - 176); i++)
  {
	auto color = myDrawInfo->drawBuffer[0 + myDrawInfo->myPixelOffset + i];
	auto sdl_color = myDrawInfo->drawPalette[color];
	tempDrawBuffer[i + 176 * SCREEN_WIDTH] = SDL_MapRGBA(myFormat, sdl_color.r, sdl_color.g, sdl_color.b, sdl_color.a);
  }
  SDL_UpdateTexture(myTexture, NULL, tempDrawBuffer, SCREEN_WIDTH*sizeof(uint32_t));
  SDL_RenderClear(myRenderer);
  SDL_RenderCopy(myRenderer, myTexture, NULL, NULL);
  SDL_RenderPresent(myRenderer);
}

           #include <sys/mman.h>
           #include <fcntl.h>
           #include <semaphore.h>
           #include <sys/stat.h>
           #include <stdio.h>
           #include <stdlib.h>
           #include <unistd.h>

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                                   } while (0)

int main()
{
  		char *shmpath = "/myDrawInfo";

		shm_unlink(shmpath);

               /* Create shared memory object and set its size to the size
                  of our structure. */

               int fd = shm_open(shmpath, O_CREAT | O_EXCL | O_RDWR,
                                 S_IRUSR | S_IWUSR);
               if (fd == -1)
                   errExit("shm_open");

               if (ftruncate(fd, sizeof(myDrawInfoS)) == -1)
                   errExit("ftruncate");

               /* Map the object into the caller's address space. */

               myDrawInfo = (myDrawInfoS *)mmap(NULL, sizeof(*myDrawInfo),
                                          PROT_READ | PROT_WRITE,
                                          MAP_SHARED, fd, 0);
               if (myDrawInfo == MAP_FAILED)
                   errExit("mmap");

	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
	  printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
    }
	myWindow = SDL_CreateWindow( "FFFF", 2880 - 2*SCREEN_WIDTH, 1800 - 2*SCREEN_HEIGHT, SCREEN_WIDTH * 2, SCREEN_HEIGHT * 2, SDL_WINDOW_BORDERLESS );
		if( myWindow == NULL )
		{
			printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
		}
		else
		{
			myRenderer = SDL_CreateRenderer(myWindow, -1, SDL_RENDERER_ACCELERATED);

			myTexture = SDL_CreateTexture(myRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

			myFormat = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);

		   while (true)
			{
			   SDL_Event e;
			   while (SDL_PollEvent(&e) > 0) {
				 switch (e.type) {
				 case SDL_QUIT:
				   return EXIT_SUCCESS;
			      }
			   }
			   updateDraw();
			   SDL_Delay(20);
			   printf("offset = %x\n", myDrawInfo->myOffset);
		   }
		}


return 0;
}
