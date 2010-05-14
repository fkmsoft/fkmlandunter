#include <SDL/SDL.h>
#include <SDL/SDL_net.h>
#include <stdlib.h>

void init_sdl()
{
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
    exit(EXIT_FAILURE);

  atexit(SDL_Quit);
}

void do_net(char *host)
{
  IPaddress addr;
  if (SDLNet_Init() < 0) {
    printf("SDLNet_Init: %s\n", SDLNet_GetError());
    exit(EXIT_FAILURE);
  }

  if (SDLNet_ResolveHost(&addr, host, 0) < 0) {
    printf("ResolveHost: %s\n", SDLNet_GetError());
    SDLNet_Quit();
    exit(EXIT_FAILURE);
  } else {
    Uint32 n;
    n = addr.host;
    printf("\n%s:\n", host);
    printf("Network-order:\t%d.%d.%d.%d\n",
      (n & 0xFF000000)>>24, (n & 0x00FF0000)>>16,
      (n & 0x0000FF00)>> 8, (n & 0x000000FF));

   n =SDLNet_Read32(&addr.host);
    printf("Host-order:\t%d.%d.%d.%d\n",
      (n & 0xFF000000)>>24, (n & 0x00FF0000)>>16,
      (n & 0x0000FF00)>> 8, (n & 0x000000FF));
    }
}

int main(int argc, char **argv)
{
  init_sdl();

  if (argc > 1) {
    while(*++argv)
      do_net(*argv);
  } else do_net("router");
  return EXIT_SUCCESS;
}
