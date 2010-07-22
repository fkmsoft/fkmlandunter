/* random.c
 *
 * Randall!
 *
 * (c) Fkmsoft, 2010
 */

#define DEFPORT (1337)
#define DEFNAME ("Randall")
#define DEFHOST ("127.0.0.1")

/* for fdopen */
#define _POSIX_SOURCE

#include <stdio.h>
/* for getopt */
#include <getopt.h>
/* #include <unistd.h> */

#include "../../curses/communication.h"

int main(int argc, char **argv) {
    bool debug = false, silent = false;
    char *name = DEFNAME, *host = DEFHOST;
    int opt, port = DEFPORT;

    while ((opt = getopt(argc, argv, "n:p:h:ds")) != -1) {
        switch (opt) {
            case 'n':
                name = optarg;
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case 'h':
                host = optarg;
                break;
            case 'd':
                debug = true;
                break;
            case 's':
                silent = true;
                break;
            default:
                printf("Usage: %s [-n name] [-h host] [-p port]\n", name);
                exit(EXIT_FAILURE);
        }
    }

    int sock = create_sock();
    FILE *fp = fdopen(sock, "a+");
    /*if (setvbuf(fp, 0, _IONBF, 0) != 0)
        perror("setvbuf");*/
    if (connect_socket(sock, host, port) == -1) {
        printf("%s not find server\n", name);
        exit(EXIT_FAILURE);
    };
    send_to(fp, "LOGIN %s\n", name);
    if (!silent)
        printf("%s connected\n", name);

    char *input = 0, *p;
    int pos = 0, points = 0, i;
    /* position */
    do {
        if (input)
            free(input);
        input = receive_from(fp);
        if (debug)
            printf("%s: %s", name, input);
    } while (strncmp(input, "START ", 6) != 0);
    /* START 5 a b c d e */
    input[strlen(input)-1] = 0;
    strtok(input, " "); 
    strtok(0, " "); 
    while (strcmp(strtok(0, " "), name) != 0) {
        pos++;
    }
    if (!silent)
        printf("%s has pos %d\n", name, pos);

    bool play = true, drowned = false;
    char *in;
    while (play) {
        if ((input = receive_from(fp)) == 0) {
            if (!silent || debug)
                printf("%s: Error on receive_from!\n", name);
            play = false;
            continue;
        }
        if (debug)
            printf("%s: %d %s", name, play, input);

        int card;
        for (in = input; in != (char *)1; in = strchr(in, '\n') + 1) {
            if (strncmp(in, "TERMINATE", 9) == 0) {
                play = false;
            } else if (strncmp(in, "DECK ", 5) == 0) {
                card = 0;
                p = in + 5;
                while (!card && *p)
                    card = atoi(p++);

                if (!silent && !card)
                    printf("%s not find card in deck\n", name);
            } else if (strncmp(in, "RINGS ", 6) == 0) {
                p = strchr(in, ' ');
                for (i = -1; i < pos; i++)
                    drowned = (atoi(p = strchr(p, ' ') + 1) == -1);

                if (!silent && drowned) {
                    printf("%s drown\n", name);
                } else if (!drowned) {
                    if (!silent)
                        printf("%s has play %d\n", name, card);
                    send_to(fp, "PLAY %d\n", card);
                }
            } else if (strncmp(in, "POINTS ", 7) == 0) {
                p = strchr(in, ' ');
                for (i = -1; i < pos; i++)
                    points = atoi(p = strchr(p, ' ') + 1);

                if (!silent)
                    printf("%s has points %d\n", name, points);
            }
        }

        free (input);
    }

    fclose(fp);
    if (!silent)
        printf("%s has finish %d %d.\n", name, points, play);

    exit(points);
}

/* vim: set sw=4 ts=4 et fdm=syntax: */
