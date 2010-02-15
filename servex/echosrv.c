/* echosrv.c
 *
 * For testing general server functionality
 *
 * (c) 2010, fkmsft
 */

#define _POSIX_SOURCE
/* for POLLRDHUP: */
#define _GNU_SOURCE

#include <sys/socket.h>
#include <sys/types.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <poll.h>

#include "fkml_server.h"
#include "queue.h"

#define PORT (1337)
#define BUFLEN (1024)
#define STDINFD (0)

#define QUITCMD ("quit")
#define PROMPT ("fkml-0.1$ ")

int echo(void);
void freak_all_clients(fkml_server *s, char *msg);
void print_pollfds(struct pollfd *p);

int main()
{
    return echo();
}

int echo()
{
    fkml_server *s = init_server(PORT, MAX_PLAYERS);
    char buf[BUFLEN];
    bool run = true;
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
        if (poll(p, 2+s->connected, -1 /* infinite t/o */) < 1) {
            run = false;
            perror("Polling failed");
        }

        /* input avail from bofh */
        if (pollfds[0].revents & POLLIN) {
            if (fgets(buf, BUFLEN-1, stdin)) {
                buf[strlen(buf)-1] = '\0';
                if (strcmp(QUITCMD, buf) == 0) {
                   run = false;
                   /* break; */
                } else if (buf[0] == '*')
                    freak_all_clients(s, buf + 1);
                else if (buf[0] == 'p')
                    q_p();
                else if (buf[0] == 'c') {
                    fkml_printclients(s);
                    print_pollfds(pollfds);
                    printf("We believe there are %d connected\n", s->connected);
                } else
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
        if (pollfds[1].revents & POLLIN) {
            /* puts("Client connecting"); */
            fkml_addclient(s,
                    pollfds[s->connected + 2].fd = accept(s->socket, 0, 0));
            /* puts("Client added, sending banner"); */
            fkml_puts(s, s->connected-1, "Welcome to the fkml server\n"
                    "Waiting for input!\n");
            /* puts("Sent banner"); */
            pollfds[1 + s->connected].events = POLLIN | POLLRDHUP;
            /* make sure we do not read some random bullshit in the next if: */
            pollfds[1 + s->connected].revents = 0;
        }

        int i;
        /* input from existing client */
        for (i = 0; i < s->connected; i++) {
            /* printf("Checking client %d\n", i); */
            if (pollfds[i+2].revents & POLLRDHUP) {
                printf("Client %d disconnected\n", i);
                fkml_rmclient(s, i);
                int j;
                for (j = i+3; j+2 < s->connected; j++)
                    pollfds[j-1] = pollfds[j];
            } else if (pollfds[i+2].revents & POLLIN) {
                /* printf("Reading from client %d\n", i); */
                char *msg;
                if ((msg = fkml_recv(s, i))) {
#if 0
                    /* printf("Read \"%s\" from client %d, echoing\n", msg, i); */
                    if (!(fkml_puts(s, i, msg))) {
                        puts("Writing to client postponed");
                        /* run = false; */
                    }
                    free(msg);
#endif
                    if (fkml_process(s, i, msg) == 3/*LOGOUT*/) {
                        printf("Client %d logged out\n", i);
                        int j;
                        for (j = i+3; j+2 < s->connected; j++)
                            pollfds[j-1] = pollfds[j];
                    }
                    free(msg);
                }
            }
        }

        /* Shutdown request or error occured */
        if(!run) {
            for (i = 0; i < s->connected; i++)
                fkml_puts(s, i, "Terminating fkml session\n");
            puts("Shutting down server");
        }
    }

    fkml_shutdown(s);

    return 0;
}

void freak_all_clients(fkml_server *s, char *msg)
{
    int i;
    for (i = 0; i < s->connected; i++) {
        fkml_puts(s, i, msg);
        fkml_puts(s, i, "\n");
    }
}

void print_pollfds(struct pollfd *p)
{
    int i;
    for (i = 0; i < MAX_PLAYERS+2; i++)
        printf("pollfd[%d]: fd = %d, evs = %d, revs = %d\n", i,
                p[i].fd, p[i].events, p[i].revents);
}

/* vim: set sw=4 ts=4 fdm=syntax: */
