#include "sdl_util.h"

void init_sdl()
{
    if (SDL_Init(SDL_INIT_EVENTTHREAD) < 0)
        exit(EXIT_FAILURE);

    atexit(SDL_Quit);

    if (SDLNet_Init() < 0) {
        printf("SDLNet_Init: %s\n", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }
}

IPaddress compute_address (char *host, Uint16 port)
{
    IPaddress addr;

    if (SDLNet_ResolveHost(&addr, host, port) < 0) {
        printf("ResolveHost: %s\n", SDLNet_GetError());
        /*return (IPaddress)0; FIXME */
    }

    return addr;
}

void send_to(TCPsocket sock, char *fmt, ...)
{
    char buf[MAXBUF];
    va_list args;
    int c, d;

    va_start(args, fmt);
    c = vsnprintf(buf, MAXBUF, fmt, args);
    if ((d = SDLNet_TCP_Send(sock, buf, c)) != c) {
        printf("Could only wrote %d of %d bytes\n", d, c);
    }

    va_end(args);
}

char *receive_from(TCPsocket sock)
{
    char *buffer = calloc(MAXBUF, sizeof(char));
    
    if (SDLNet_TCP_Recv(sock, buffer, MAXBUF) < 1) {
        printf("receive_from: %s\n", SDLNet_GetError());
        return NULL;
    }

    return buffer;
}

/* vim: set sw=4 ts=4 et fdm=syntax: */
