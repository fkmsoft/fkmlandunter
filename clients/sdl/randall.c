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

static void render(SDL_Surface *screen, gamestr *g, char *name, int pos, double hs, double vs);

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
    double hs, vs;
    char buf[BUFL];

    if (gui) {
        hs = h / (float) H;
        vs = w / (float) W;

        screen = init_sdl(w, h);

        draw_background(screen, 0, 0);
        draw_hud(screen, 0, 0);
        create_playerbox(screen, name, hs * pbox_x, vs * pbox_y, 0, 0, false);
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

        x = 50 * hs;
        y = 10 * hs;
        for (i = 0; i < g->count; i++) {
            if (strcmp(g->villain[i].name, name) == 0) {
                pos = i;
                if (debug)
                    printf("Have pos %d\n", pos);
            } else {
                if (debug)
                    printf("Have opponent %s\n", g->villain[i].name);
                create_playerbox(screen, g->villain[i].name, x, y, 0, 0, false);
                x += 200 * hs;
            }
        }

        SDL_UpdateRect(screen, 0, 0, 0, 0);
    }

    SDL_Event ev;
    bool play = true, drowned = false;
    int card = 0;
    char *in;
    while (play) {
        if (gui)
            render(screen, g, name, pos, hs, vs);

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

        for (in = input; in != (char *)1; in = strchr(in, '\n') + 1) {
            if (!gui && !strncmp(in, "START ", 6)) {
                p = strchr(in, ' ') + 1;
                i = 0;
                for (p = strchr(p, ' ') + 1; strncmp(p, name, strlen(name)); i++, p = strchr(p, ' ') + 1)
                    ;
                pos = i;
                if (debug)
                    printf("%s: pos %d\n", name, pos);
            } else if (!strncmp(in, "TERMINATE", 9)) {
                play = false;
                if (!silent)
                    printf("%s good-bye\n", name);
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
            }
        }

        for (p = input; (p - 1) && *p; p = strchr(p, '\n') + 1) {
            strncpy(buf, p, BUFL);
            buf[BUFL - 1] = 0;
            if (debug && 0)
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

static void render(SDL_Surface *screen, gamestr *g, char *name, int pos, double hs, double vs)
{
    int x, y, i, j;
    player p;

    draw_background(screen, 0, 0);

    p = g->villain[pos];
    add_pcard_played(screen, 0, 0, -1, p.played);
    draw_hud(screen, 0, 0);

    create_playerbox(screen, name, hs * pbox_x, vs * pbox_y, 0, 0, p.dead);
    set_wlevel(screen, hs * pbox_x, vs * pbox_y, p.water_level);
    set_points(screen, hs * pbox_x, vs * pbox_y, p.points);
    set_lifebelts(screen, hs * pbox_x, vs *pbox_y, p.lifebelts, 10);
    add_wcard(screen, 0, 0, 0, g->w_card[0]);
    add_wcard(screen, 0, 0, 1, g->w_card[1]);

    x = 50 * hs;
    y = 10 * vs;
    j = 0;
    for (i = 0; i < g->count; i++) {
        if (i != pos) {
            p = g->villain[i];
            create_playerbox(screen, p.name, x, y, 0, 0, p.dead);
            set_wlevel(screen, x, y, p.water_level);
            set_points(screen, x, y, p.points);
            set_lifebelts(screen, x, y, p.lifebelts, 10);
            x += 200 * hs;
            add_pcard_played(screen, 0, 0, j++, p.played);
        }
    }

    for (i = 0; i < 12; i++)
        if (g->player.weathercards[i])
            add_pcard(screen, 0, 0, i, g->player.weathercards[i]);

    SDL_UpdateRect(screen, 0, 0, 0, 0);
}

/* vim: set sw=4 ts=4 et fdm=syntax: */
