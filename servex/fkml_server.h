/*
 * fkmsft
 */

#ifndef FKML_SERVER
#define FKML_SERVER

#include <stdio.h>

#define MAX_PLAYERS (5)

typedef struct {
    int socket; /* socket for accepting new clients */
    int connected; /* number of connected clients */
    FILE *clients[MAX_PLAYERS]; /* FILE handles of connected clients */
    int clientfds[MAX_PLAYERS]; /* sockets of connected clients */
} fkml_server;

/* Create a fkmlandunter for players players on port port */
fkml_server *init_server(unsigned int port, unsigned int players);

/* Add a new client described by file descriptor fd to server s */
void fkml_addclient(fkml_server *s, int fd);

/* Remove client c from server s */
void fkml_rmclient(fkml_server *s, int c);

/* Receive message from client c on server s */
char *fkml_recv(fkml_server *s, int c);

/* Send message msg to client c on server s */
int fkml_puts(fkml_server *s, int c, char *msg);

/* Print debug info about client c on server s */
void fkml_printclients(fkml_server *);

/* Shut down server s */
void fkml_shutdown(fkml_server *s);

#endif /* FKML_SERVER */
/* vim: set sw=4 ts=4 fdm=syntax: */
