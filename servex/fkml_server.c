/* fkmsft
 *
 * fkmlandunter prtcl:
 * -----------------------------
 * -> LOGIN name
 * <- ACK nam
 * <- START 3 nam1 nam2 nam3
 *
 * <- WEATHER 7 8
 * <- DECK 3 1 2 56
 * -> PLAY 56
 * <- ACk 56
 * <- FAIL 56
 * <- WLEVELS 7 8 0
 * <- POINTS 1 2 -1
 *
 * -> LOGOUT bye
 * <- TERMINATE fuck off
 *
 * -> MSG fu all los0rZ
 * <- MSGFROM name fu all los0rZ
 * -----------------------------
 */

/* for fdopen() from stdio */
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
/* for socket(): */
#include <sys/types.h>
#include <sys/socket.h>
/* for memset(): */
#include <string.h>
/* for struct sockaddr_in: */
#include <netinet/in.h>
/* for close(): */
#include <unistd.h>

#include "fkml_server.h"

fkml_server *init_server(unsigned int port, unsigned int players)
{
    if (players > MAX_PLAYERS)
        players = MAX_PLAYERS;

    fkml_server *server = malloc(sizeof(fkml_server));

    if (port < 1024)
        printf("Please choose a port > 1024 instead of %d\n",
                port);

    server->socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in s_addr;
    memset(&s_addr, 0, sizeof(s_addr));

    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = INADDR_ANY;
    s_addr.sin_port = htons(port);

    bind(server->socket, (struct sockaddr *) &s_addr, sizeof(s_addr));
    listen(server->socket, players); /* only 1 connect() at a time */

    int i, client, count = 0;
    puts("Init done, waiting for clients...");
    for (i = 0; i < players; i++) {
        client = accept(server->socket, 0, 0);

        FILE *clientstream;
        if(!(clientstream = (FILE *)fdopen(client, "a+")))
            perror("Error opening stream");

        fputs("Welcome to the fkml server\n", clientstream);
        fprintf(clientstream, "%d players have already connected"
            " to this server.\n", count);
        /* fgetc(clientstream); */
        /* fclose(clientstream); */
        printf("client No %d connected\n", ++count);
        server->clientfds[i] = client;
        server->clients[i] = clientstream;
    }

    return server;
}

void fkml_shutdown(fkml_server *s)
{
    int i;
    /*FILE *p;

    p = s->clients[0];*/
    for (i = 0/*, p = s->clients[0]*/; i < MAX_PLAYERS && s->clients[i]; i++)
            fclose(s->clients[i]);

    close(s->socket);

    free(s);
}

/* vim: set sw=4 ts=4 fdm=syntax: */
