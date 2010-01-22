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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> // socket()
#include <sys/socket.h> // socket()
#include <string.h> // memset()
#include <netinet/in.h> // struct sockaddr_in

#include "util.h"

fkml_server *init_server(unsigned int port)
{
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

    bind(server.socket, (struct sockaddr *) &s_addr, sizeof(s_addr));
    listen(server->socket, MAX_PLAYERS); // only 1 connect() at a time

    int client, count = 0;
    puts("Init done, waiting for clients...");
    while(1) {
        client = accept(server->socket, 0, 0);

        FILE *clientstream;
        clientstream = fdopen(client, "a");
        fputs("Welcome to the ride on my disco stick\n", clientstream);
        fprintf(clientstream, "%d people had their ride before"
            "you on this server.\n", count);
        fgetc(clientstream);
        fclose(clientstream);
        printf("ride No %d finished\n", ++count);
        if (!(count % 10))
            puts("Please replace the disco stick NOW!");
    }

    return server;
}

/* vim: set sw=4 ts=4 fdm=syntax: */
