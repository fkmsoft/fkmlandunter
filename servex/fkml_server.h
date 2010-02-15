/*
 * fkmsft
 */

#ifndef FKML_SERVER
#define FKML_SERVER

#include <stdarg.h>

#include "../ncom.h"

#define MAX_PLAYERS (5)

#if 0
typedef struct {
    int socket; /* socket for accepting new clients */
    int connected; /* number of connected clients */
#if 0
    FILE *clients[MAX_PLAYERS]; /* FILE handles of connected clients */
    int clientfds[MAX_PLAYERS]; /* sockets of connected clients */
#endif
    player players[MAX_PLAYERS];
} fkml_server;
#endif

/* Create a fkmlandunter for players players on port port */
fkml_server *init_server(unsigned int port, unsigned int players);

/* Wait for a new player to connect and add it to server s */
void fkml_addplayer(fkml_server *s);

/* Add a new client described by file descriptor fd to server s */
void fkml_addclient(fkml_server *s, int fd);

/* Remove client c from server s */
void fkml_rmclient(fkml_server *s, int c);

/* Receive message from client c on server s */
char *fkml_recv(fkml_server *s, int c);

/* Send message msg to client c on server s */
int fkml_puts(fkml_server *s, int c, char *msg);

void fkml_printf(fkml_server *s, int c, char *msg, ...);

/* process message msg from client c on server s */
int fkml_process(fkml_server *s, int c, char *msg);

/* Handle command cmd with arguments args in server s */
int fkml_handle_command(fkml_server *s, char *cmd, char *args);

/* Print debug info about client c on server s */
void fkml_printclients(fkml_server *);

/* Shut down server s */
void fkml_shutdown(fkml_server *s);

#endif /* FKML_SERVER */

/* vim: set sw=4 ts=4 fdm=syntax: */
