/* net_util.h
 *
 * (c) 2010, Fkmsoft
 */

#ifndef FKML_NET_UTIL_H
#define FKML_NET_UTIL_H

/* for vsnprintf */
#define _ISOC99_SOURCE
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

#include <SDL/SDL.h>
#include <SDL/SDL_net.h>

#define MAXBUF (1024)


void net_init_sdl();
IPaddress compute_address (char *host, Uint16 port);

void sdl_send_to(TCPsocket sock, char *fmt, ...);
char *sdl_receive_from(TCPsocket sock, bool debug);

#endif /* FKML_NET_UTIL_H */

/* vim: set sw=4 ts=4 et fdm=syntax: */
