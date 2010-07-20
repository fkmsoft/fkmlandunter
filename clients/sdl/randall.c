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
#include "../curses/communication.h"

#define W 800
#define H 600
#define DEFPORT (1337)
#define DEFNAME ("Randall")
#define GUINAME ("Guilord")
#define DEFHOST ("127.0.0.1")

#define BUFL 512

int main(int argc, char **argv) {
    bool debug = false, silent = false, gui = false;
    char *name = DEFNAME, *host = DEFHOST;
    int opt, port = DEFPORT, w = W, h = H;

    while ((opt = getopt(argc, argv, "n:p:h:dsgr:")) != -1) {
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
            w = atoi(optarg);
            h = atoi(strchr(optarg, 'x') + 1);
            printf("have %dx%d\n", w, h);
            break;
        default:
            printf("Usage: %s [-n name] [-h host] [-p port]\n", name);
            exit(EXIT_FAILURE);
        }
    }

    if (gui && !strcmp(name, DEFNAME))
        name = GUINAME;

    /* initialize sdl & sdl_net */
    SDL_Surface *screen;
    gamestr *g;
    double hs, vs;
    char buf[BUFL];

    if (gui) {
        hs = h / (float) H;
        vs = w / (float) W;

        screen = init_sdl(w, h);
        draw_hud(screen, 0, 0);
        create_playerbox(screen, name, hs * pbox_x, vs * pbox_y, 0, 0);
        set_wlevel(screen, 0, 0, 0);
        set_points(screen, 0, 0, 0);
        SDL_UpdateRect(screen, 0, 0, 0, 0);
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

    /* for gui */
    int x, y;
    double hstr, vstr;

    /* position */
    do {
        if (input)
            free(input);
        input = sdl_receive_from(sock);
        if (debug)
            printf("%s: %s", name, input);
    } while ( !((p = strstr(input, "START ")) && (p == input || *(p - 1) == '\n')) );

    if (gui) {
        hstr = w / W;
        vstr = h / H;
        strncpy(buf, p, BUFL);
        parse_start(g, buf);

        x = 50 * hstr;
        y = 0;
        for (i = 0; i < g->count; i++) {
            if (strcmp(g->villain->name, name) == 0) {
                pos = i;
                if (debug)
                    fprintf(stderr, "Have pos %d\n", pos);
            } else {
                create_playerbox(screen, g->villain[i].name, x, y, 0, 0);
                x += 200 * hstr;
            }
        }

        SDL_UpdateRect(screen, 0, 0, 0, 0);
    }

    bool play = true;
    while (play) {
        if (gui) {
            draw_hud(screen, 0, 0);
            create_playerbox(screen, name, hs * pbox_x, vs * pbox_y, 0, 0);
            set_wlevel(screen, hs * pbox_x, vs * pbox_y, g->villain[pos].water_level);
            set_points(screen, hs * pbox_x, vs * pbox_y, g->villain[pos].points);
            set_lifebelts(screen, hs * pbox_x, vs *pbox_y, g->villain[pos].lifebelts, 10);
            add_wcard(screen, 0, 0, 0, g->w_card[0]);
            add_wcard(screen, 0, 0, 1, g->w_card[1]);

            x = 50 * hstr;
            y = 0;
            for (i = 0; i < g->count; i++) {
                if (i != pos) {
                    create_playerbox(screen, g->villain[i].name, x, y, 0, 0);
                    set_wlevel(screen, x, y, g->villain[i].water_level);
                    set_points(screen, x, y, g->villain[i].points);
                    set_lifebelts(screen, x, y, g->villain[i].lifebelts, 10);
                    x += 200 * hstr;
                }
            }

            for (i = 0; i < 12; i++)
                if (g->player.weathercards[i])
                    add_pcard(screen, 0, 0, i, g->player.weathercards[i]);

            SDL_UpdateRect(screen, 0, 0, 0, 0);
        }

        if (!input) {
            if ((input = sdl_receive_from(sock)) == 0) {
                if (!silent || debug)
                    printf("%s: Error on sdl_receive_from!\n", name);
                play = false;
                points = 113;
                puts("FUCK");
                continue;
            }
        } else {
            /* parse the rest for first turn */
            if (debug)
                printf("%s: parsing the rest of it: %s\n", name, input);
        }
        if (debug)
            printf("%s: %s", name, input);

        if (strstr(input, "TERMINATE"))
            play = false;
        if ((p = strstr(input, "DECK "))) {
            int card = 0;
            p += 5;
            while (!card && *p)
                card = atoi(p++);
            if (card) {
                if (!silent)
                    printf("%s has play %d\n", name, card);
                sdl_send_to(sock, "PLAY %d\n", card);
            } else if (!silent)
                printf("%s not find card in deck\n", name);
        }
        if ((p = strstr(input, "POINTS "))) {
            strtok(p, " ");
            /*strtok(0, " ");*/
            for (i = -1; i < pos; i++)
                points = atoi(strtok(NULL, " "));
            if (!silent)
                printf("%s has points %d\n", name, points);
        }

        for (p = input; (p - 1) && *p; p = strchr(p, '\n') + 1) {
            strncpy(buf, p, BUFL);
            buf[BUFL - 1] = 0;
            if (debug)
                printf("Fed the parser with: >>%s<<\n", buf);
            parse_cmd(g, buf);
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
