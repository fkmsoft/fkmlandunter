/* server.c
 *
 * The Fkmlandunter server main function.
 *
 * (c) 2010, Fkmsoft
 */

#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <stdio.h>

#include "fkmserver.h"
#include "fkmlandunter.h"

#define MAX_PLAYERS (5)
#define DEFPORT (1337)

void usage(char *progname);

int getfd(player *p);

int main(int argc, char **argv)
{
    bool debug = false;
    int opt,
        playerlimit = MAX_PLAYERS,
        games = 1, port = DEFPORT;

    while ((opt = getopt(argc, argv, "dl:p:g:h")) != -1)
        switch (opt) {
            case 'd':
                debug = true;
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case 'l':
                playerlimit = atoi(optarg);
                break;
            case 'g':
                if (strcmp(optarg, "unlimited") == 0)
                    games = -1;
                else
                    games = atoi(optarg);
                break;
            case 'h':
                usage(argv[0]);
                exit(EXIT_SUCCESS);
            default:
                usage(argv[0]);
                exit(EXIT_FAILURE);
        }

    if (debug)
        printf("Starting server on port %d for %d games\n",
                port, games);
    fkmserver *s = fkmserver_init(port, (void *)getfd);

    int i;
    for (i = games; i != 0; i--) {
        fkmlandunter_play(s, playerlimit);
        if (debug)
            printf("Game %d/%d finished, %d clients connected\n",
                    games - i + 1, games, s->connections);
    }

    fkmserver_stop(s);

#if BLOAT
    puts("Thanks for using bloat-control!");
#else
    puts("NO Bloat used.");
#endif

    exit(EXIT_SUCCESS);
}

void usage(char *progname)
{
    printf("Usage: %s [-p port] [-l playerlimit] [-g games|unlimited]\n",
            progname);

    return;
}

int getfd(player *p)
{
    return p->fd;
}

/* vim: set sw=4 ts=4 et fdm=syntax: */
