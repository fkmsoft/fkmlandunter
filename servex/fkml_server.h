/*
 * fkmsft
 */

#ifndef FKML_SERVER
#define FKML_SERVER

#include <stdio.h>

#define MAX_PLAYERS (5)

typedef struct {
    int socket;
    FILE *clients[MAX_PLAYERS];
    int clientfds[MAX_PLAYERS];
} fkml_server;

fkml_server *init_server(unsigned int port, unsigned int players);
void fkml_shutdown(fkml_server *s);

#endif /* FKML_SERVER */
/* vim: set sw=4 ts=4 fdm=syntax: */
