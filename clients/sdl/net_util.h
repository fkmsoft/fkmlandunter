/* for vsnprintf */
#define _ISOC99_SOURCE
#include <stdio.h>
#include <stdarg.h>

#include <SDL/SDL.h>
#include <SDL/SDL_net.h>

#define MAXBUF (1024)


void init_sdl();
IPaddress compute_address (char *host, Uint16 port);

void send_to(TCPsocket sock, char *fmt, ...);
char *receive_from(TCPsocket sock);

/* vim: set sw=4 ts=4 et fdm=syntax: */
