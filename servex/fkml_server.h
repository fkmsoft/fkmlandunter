/*
 * fkmsft
 */

#ifndef FKML_SERVER
#define FKML_SERVER

#define MAX_PLAYERS (5)

typedef struct {
    int socket;
    FILE *clients[5];
} fkml_server;

fkml_server *init_server(int port);
void shutdown(fkml_server s);

#endif /* FKML_SERVER */
/* vim: set sw=4 ts=4 fdm=syntax: */
