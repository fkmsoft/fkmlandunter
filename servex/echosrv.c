/* echosrv.c
 *
 * For testing general server functionality
 *
 * (c) 2010, fkmsft
 */

#define _POSIX_SOURCE

#include <sys/socket.h>
#include <sys/types.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <poll.h>

#include "fkml_server.h"

#define PORT (1337)
#define BUFLEN (1024)
#define STDINFD (0)

#define QUITCMD ("quit")
#define PROMPT ("fkml-0.1$ ")

/* for POLLRDHUP: */
/*#define _GNU_SOURCE*/

int main()
{
    fkml_server *s = init_server(PORT, MAX_PLAYERS);
    char buf[BUFLEN];
    bool run = true;
    int connected = 0;
    struct pollfd pollfds[MAX_PLAYERS+2], *p;
    p = pollfds;
    pollfds[0].fd = STDINFD;
    pollfds[0].events = POLLIN;
    pollfds[1].fd = s->socket;
    pollfds[1].events = POLLIN;

    /* printf("EOF is %d\n", EOF); */
    fputs(PROMPT, stdout);
    fflush(stdout);

    while(run) {
        /* puts("Trying poll"); */
        if (poll(p, 2, -1 /* infinite t/o */) < 1) {
            run = false;
            perror("Polling failed");
        }
        if (pollfds[0].revents == POLLIN) /* input avail from bofh */ {
            if (fgets(buf, BUFLEN-1, stdin)) {
                buf[strlen(buf)-1] = '\0';
                if (strcmp(QUITCMD, buf) == 0) {
                   run = false;
                   /* break; */
                }
                else
                    printf("Invalid command %s, use \"%s\" to quit\n",
                            buf, QUITCMD);

                if (run) {
                    fputs(PROMPT, stdout);
                    fflush(stdout);
                }
            } else {
                perror("read from stdin failed");
                run = false;
            }
        }

        if (pollfds[1].revents == POLLIN) { /* new client connecting */
            s->clientfds[connected] = accept(s->socket, 0, 0);
            s->clients[connected] = fdopen(s->clientfds[connected], "a+");
            fputs("Welcome to the fkml server\n", s->clients[connected]);
            fputs("Waiting for input!\n", s->clients[connected]);
            fflush(s->clients[connected]);
            connected++;
        }

        int i;
        for (i = 0; i < connected; i++) {
            printf("Checking client %d\n", i);
            if (pollfds[i+2].revents == POLLIN)
                printf("Reading from client %d\n", i);
                if (fgets(buf, BUFLEN-1, s->clients[i])) {
                    printf("Read \"%s\" from client %d, echoing\n", buf, i);
                    if (!(fputs(buf, s->clients[i]))) {
                        puts("Writing to client failed");
                        run = false;
                    }
                    fflush(s->clients[i]);
                    puts("Echo complete");
                }
        }

        if(!run) {
            for (i = 0; i < connected; i++)
                fputs("Terminating\n", s->clients[i]);
            puts("Shutting down server");
        }
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
