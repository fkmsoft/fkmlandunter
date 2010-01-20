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
#include <sys/types.h> // socket()
#include <sys/socket.h> // socket()
#include <string.h> // memset()
#include <netinet/in.h> // struct sockaddr_in

#include "util.h"

void run_server(int port)
{
    int sock;

    if (port < 1024)
        printf("Please choose a port > 1024 instead of %d\n",
                port);

    sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in s_addr;
    memset(&s_addr, 0, sizeof(s_addr));

    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = INADDR_ANY;
    s_addr.sin_port = htons(port);

    bind(sock, (struct sockaddr *) &s_addr, sizeof(s_addr));
    listen(sock, 5);

    int client;
    puts("Init done, waiting for a client...");
    client = accept(sock, 0, 0);

    write(client, "Welcome to the ride on my disco stick\n", 39);
}

/* vim: set sw=4 ts=4 fdm=syntax: */
