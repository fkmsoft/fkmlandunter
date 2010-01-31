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

void freak_all_clients(fkml_server *s, int num_clients, char *msg);

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
        /* puts("Beginning new poll cycle"); */
        if (poll(p, 2+connected, -1 /* infinite t/o */) < 1) {
            run = false;
            perror("Polling failed");
        } else {
#if 0
            /* Poll successful, print what we got: */
            fputs("Poll found input from", stdout);
            if (pollfds[0].revents == POLLIN)
                fputs(" server terminal,", stdout);
            if (pollfds[1].revents == POLLIN)
                fputs(" server listening socket,", stdout);
            int i;
            for (i = 0; i < connected; i++)
                if (pollfds[2+i].revents == POLLIN)
                    printf(" client %d", i);
            puts(" and nobody else.");
#endif
        }

        /* input avail from bofh */
        if (pollfds[0].revents == POLLIN) {
            if (fgets(buf, BUFLEN-1, stdin)) {
                buf[strlen(buf)-1] = '\0';
                if (strcmp(QUITCMD, buf) == 0) {
                   run = false;
                   /* break; */
                } else if (buf[0] == '*')
                    freak_all_clients(s, connected, buf + 1);
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

        /* new client connecting */
        if (pollfds[1].revents == POLLIN) {
            pollfds[connected + 2].fd = s->clientfds[connected] =
                accept(s->socket, 0, 0);
            s->clients[connected] = fdopen(s->clientfds[connected], "a+");
            fputs("Welcome to the fkml server\n", s->clients[connected]);
            fputs("Waiting for input!\n", s->clients[connected]);
            fflush(s->clients[connected]);
            pollfds[2 + connected].events = POLLIN;
            connected++;
        }

        int i;
        /* input from existing client */
        for (i = 0; i < connected; i++) {
            /* printf("Checking client %d\n", i); */
            if (pollfds[i+2].revents == POLLIN) {
                /* printf("Reading from client %d\n", i); */
                if (fgets(buf, BUFLEN-1, s->clients[i])) {
                    /* printf("Read \"%s\" from client %d, echoing\n", buf, i); */
                    if (!(fputs(buf, s->clients[i]))) {
                        puts("Writing to client failed");
                        run = false;
                    }
                    fflush(s->clients[i]);
                    /* puts("Echo complete"); */
                }
            }
        }

        /* Shutdown request or error occured */
        if(!run) {
            for (i = 0; i < connected; i++)
                fputs("Terminating\n", s->clients[i]);
            puts("Shutting down server");
        }
    }

    fkml_shutdown(s);

    return 0;
}

void freak_all_clients(fkml_server *s, int num_clients, char *msg)
{
    int i;
    for (i = 0; i < num_clients; i++) {
        fputs(msg, s->clients[i]);
        fputc('\n', s->clients[i]);
        fflush(s->clients[i]);
    }
}

/* vim: set sw=4 ts=4 fdm=syntax: */
