/* randall.c - SDL random ai player,
 * needs libsdl and sdl_net to work.
 *
 * (c) Fkmsoft, 2010
 */
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <getopt.h>

#include "net_util.h"
#include "gui_util.h"

#define W 800
#define H 600
#define DEFPORT (1337)
#define DEFNAME ("Randall")
#define GUINAME ("Guilord")
#define DEFHOST ("127.0.0.1")

#define BUFL 512

int main(int argc, char **argv) {
    bool debug = false, silent = false, gui = false, interactive = false;
    char *name = DEFNAME, *host = DEFHOST;
    int opt, port = DEFPORT, w = W, h = H;

    while ((opt = getopt(argc, argv, "n:p:h:dsgr:i")) != -1) {
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
        case 'g':
            gui = true;
            break;
        case 'r':
            gui = true;
            w = atoi(optarg);
            h = atoi(strchr(optarg, 'x') + 1);
            if (debug)
                printf("have %dx%d\n", w, h);
            break;
        case 'i':
            gui = true;
            interactive = true;
            break;
        default:
            printf("Usage: %s [-d] [-s] [-g] [-i] [-n name] [-h host] [-p port] [-r 800x600]\n", name);
            exit(EXIT_FAILURE);
        }
    }

    if (gui && !strcmp(name, DEFNAME))
        name = GUINAME;

    /* initialize sdl & sdl_net */
    SDL_Surface *screen;
    gamestr *g;
    char buf[BUFL];
    int startbelts[5];
    if (gui) {
        screen = init_sdl(w, h, 0, 0);

        pre_render(screen, name);

        startbelts[0] = -1;

        g = create_game();
    }

    /* compute address & connect */
    IPaddress addr = compute_address(host, port);

    TCPsocket sock = SDLNet_TCP_Open(&addr);
    if (sock == NULL) {
        printf("%s cannot connect to %s\n", name, host);
        exit(EXIT_FAILURE);
    }

    sdl_send_to(sock, "LOGIN %s\n", name);
    if (!silent)
        printf("%s connected\n", name);

    char *input = 0, *p;
    int pos = -1, points = 0, i;

    /* position */
    do {
        if (input)
            free(input);
        input = sdl_receive_from(sock, debug);
        if (debug)
            printf("%s: %s", name, input);
    } while ( !((p = strstr(input, "START ")) && (p == input || *(p - 1) == '\n')) );

    if (gui) {
        strncpy(buf, p, BUFL);
        parse_start(g, buf);

        render(screen, g, pos, startbelts);
        SDL_UpdateRect(screen, 0, 0, 0, 0);
    }

    SDL_Event ev;
    bool play = true, drowned = false;
    int card = 0;
    char *in;
    while (play) {

        if (!input) {
            if ((input = sdl_receive_from(sock, debug)) == 0) {
                if (!silent || debug)
                    printf("%s: Error on sdl_receive_from!\n", name);
                play = false;
                continue;
            }
        } else {
            /* parse the rest for first turn */
            if (debug)
                printf("%s: parsing the rest of it: %s\n", name, input);
        }
        if (debug)
            printf("%s: %s", name, input);

        for (p = input; (p - 1) && *p; p = strchr(p, '\n') + 1) {
            strncpy(buf, p, BUFL);
            buf[BUFL - 1] = 0;
            if (debug && 0)
                printf("Fed the parser with: >>%s<<\n", buf);
            parse_cmd(g, buf);
        }

        if (gui) {
            if (startbelts[0] == -1) {
                for (i = 0; i < g->count; i++)
                    startbelts[i] = g->villain[i].lifebelts;
                if (debug)
                    printf("Assigned startbelts: %d %d\n", startbelts[0], startbelts[1]);
            }

            render(screen, g, pos, startbelts);
            SDL_UpdateRect(screen, 0, 0, 0, 0);
        }

        for (in = input; in != (char *)1; in = strchr(in, '\n') + 1) {
            if (!strncmp(in, "START ", 6)) {
                p = strchr(in, ' ') + 1;
                i = 0;
                for (p = strchr(p, ' ') + 1; strncmp(p, name, strlen(name)); i++, p = strchr(p, ' ') + 1)
                    ;
                pos = i;
                if (debug)
                    printf("%s: pos %d\n", name, pos);
            } else if (!strncmp(in, "TERMINATE", 9)) {
                play = false;
            } else if (!strncmp(in, "DECK ", 5)) {
                p = in + 5;
                card = 0;
                while (!card && *p)
                    card = atoi(p++);

                if (!silent && !card)
                    printf("%s not find card in deck\n", name);
            } else if (!strncmp(in, "RINGS ", 6)) {
                p = strchr(in, ' ');
                for (i = -1; i < pos; i++)
                    drowned = (atoi(p = strchr(p, ' ') + 1) == -1);

                if (!silent && drowned) {
                    printf("%s drown\n", name);
                } else if (!drowned) {
                    if (!silent)
                        printf("%s has play %d\n", name, card);
                    sdl_send_to(sock, "PLAY %d\n", card);

                    if (interactive)
                        do { SDL_PollEvent(&ev); } while (ev.type != SDL_KEYDOWN);
                }

                if (startbelts[0] == 0)
                    for (i = 0; i < g->count; i++)
                        startbelts[i] = g->villain[i].lifebelts;
            } else if (!strncmp(in, "POINTS ", 7)) {
                p = strchr(in, ' ');
                for (i = -1; i < pos; i++)
                    points = atoi(p = strchr(p, ' ') + 1);

                if (!silent)
                    printf("%s has points %d\n", name, points);
                if (drowned && 0) {
                    drowned = false;
                    if (!silent)
                        printf("%s replay %d\n", name, card);
                    sdl_send_to(sock, "PLAY %d\n", card);
                }

                if (gui)
                    startbelts[0] = -1;
            }
        }

        free (input);
        input = 0;
    }

    SDLNet_TCP_Close(sock);
    if (!silent)
        printf("%s has finish %d.\n", name, points);

    exit(points);
}

/* vim: set sw=4 ts=4 et fdm=syntax: */
