/* echosrv.c
 *
 * For testing general server functionality
 *
 * (c) 2010, fkmsft
 */

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <poll.h>

#include "fkml_server.h"

#define PORT (1337)
#define BUFLEN (1024)
#define STDINFD (0)

#define QUITCMD ("quit")

/* for POLLRDHUP: */
/*#define _GNU_SOURCE*/

int main()
{
    fkml_server *s = init_server(PORT, 1);
    char buf[BUFLEN];
    bool run = true;
    struct pollfd pollfds[2], *p;
    p = pollfds;
    pollfds[0].fd = STDINFD;
    pollfds[0].events = POLLIN;
    pollfds[1].fd = s->clientfds[0];
    pollfds[1].events = POLLIN;

    printf("EOF is %d\n", EOF);
    /* fflush(stdout); */
    fputs("Waiting for input!\n", s->clients[0]);
    fflush(s->clients[0]);

    while(run) {
        puts("Trying poll");
        if (poll(p, 2, -1 /* infinite t/o */) < 1) {
            run = false;
            perror("Polling failed");
        }
        if (pollfds[0].revents == POLLIN) /* input avail from bofh */ {
            if (fgets(buf, BUFLEN-1, stdin)) {
                buf[strlen(buf)-1] = '\0';
                if (strcmp(QUITCMD, buf) == 0)
                   run = false;
                else
                    printf("Invalid command %s, use \"%s\" to quit\n",
                            buf, QUITCMD);
            } else {
                perror("read from stdin failed");
                run = false;
            }
        }

        if (pollfds[1].revents > 0 /*== POLLIN*/) {/* input avail from client */
            puts("Reading from client");
            if (fgets(buf, BUFLEN-1, s->clients[0])) {
                printf("Read \"%s\" from client, echoing\n", buf);
                if (!(fputs(buf, s->clients[0]))) {
                    puts("Writing to client failed");
                    run = false;
                }
                fflush(s->clients[0]);
                puts("Echo complete");
            }
        }

        if(!run)
            fputs("Terminating\n", s->clients[0]);
    }

    /*
    while (fgets(buf, BUFLEN-1, s->clients[0])) {
        printf("Read %s from client\n", buf);
        if (!(fputs(buf, s->clients[0])))
            puts("Writing to client failed");
    }
    */

    fkml_shutdown(s);

    return 0;
}
