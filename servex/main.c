#include <stdio.h>

#include "fkml_server.h"

int main()
{
    int i;
    fkml_server *server = init_server(1222, MAX_PLAYERS);
    for (i = 0; i<MAX_PLAYERS; i++)
        fprintf(server->clients[i], "Welcome to the fkmlandunter server\n");
    fkml_shutdown(server);
    return 0;
}
/* vim: set sw=4 ts=4 fdm=syntax: */
