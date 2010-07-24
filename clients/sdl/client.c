/* client.c
 *
 * the real sdl client
 *
 * (c) Fkmsoft, 2010
 */

#include <getopt.h>

#include "net_util.h"
#include "gui_util.h"

#define DEFPORT (1337)
#define DEFNAME ("SDL_Player")
#define DEFHOST ("127.0.0.1")

#define BUFL 1024

static const int W = 800;
static const int H = 600;

static void pre_render(SDL_Surface *s, char *name, double hs, double vs);
static void parse_input(char *input, gamestr *g, int *startbelts, int pos, SDL_Surface *s, bool rend);

int main(int argc, char **argv)
{
    bool debug = false;
    bool play = true;
    char *name = DEFNAME, *host = DEFHOST;
    char *input, *p, buf[BUFL];
    int opt, port = DEFPORT, w = W, h = H, pos, i;
    int startbelts[5], *deck;
    int down = 0;
    double hs, vs;
    gamestr *g;
    SDL_Surface *screen;
    SDL_Event ev;
    SDLNet_SocketSet set;

    while ((opt = getopt(argc, argv, "Hn:p:h:dr:")) != -1) {
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
        case 'r':
            w = atoi(optarg);
            h = atoi(strchr(optarg, 'x') + 1);
            if (debug)
                printf("have %dx%d\n", w, h);
            break;
        case 'H':
            printf("Usage: %s [-d] [-n name] [-h host] [-p port] [-r 800x600]\n", argv[0]);
            exit(EXIT_SUCCESS);
        default:
            printf("Usage: %s [-d] [-n name] [-h host] [-p port] [-r 800x600]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    hs = h / (float) H;
    vs = w / (float) W;

    screen = init_sdl(w, h);

    pre_render(screen, name, hs, vs);
    SDL_UpdateRect(screen, 0, 0, 0, 0);

    g = create_game();
    g->player.name = name;

    IPaddress addr = compute_address(host, port);

    TCPsocket sock = SDLNet_TCP_Open(&addr);
    if (sock == NULL) {
        printf("cannot connect to %s:%d\n", host, port);
        exit(EXIT_FAILURE);
    }

    sdl_send_to(sock, "LOGIN %s\n", name);

    set = SDLNet_AllocSocketSet(1);
    if (!set) {
        fprintf(stderr, "Could not allocate socket set: %s\n", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }

    SDLNet_TCP_AddSocket(set, sock);

    deck = g->player.weathercards;
    for (i = 0; i < 5; i++)
        startbelts[i] = 0;

    /* wait for START */
    play = false;
    pos = -1;
    while (!play) {
        pre_render(screen, name, hs, vs);
        SDL_UpdateRect(screen, 0, 0, 0, 0);

        SDL_PollEvent(&ev);
        if (ev.type == SDL_KEYUP && ev.key.keysym.sym == SDLK_q)
            exit(EXIT_SUCCESS);

        if (SDLNet_CheckSockets(set, 200) && SDLNet_SocketReady(sock)) {
            input = sdl_receive_from(sock, debug);
            p = strstr(input, "\nSTART");

            if (strncmp(input, "START ", 6) == 0) {
                strncpy(buf, input, BUFL);
                pos = parse_start(g, buf);
                play = true;
            } else if (p) {
                strncpy(buf, p + 1, BUFL);
                pos = parse_start(g, buf);
                play = true;
            }

            parse_input(input, g, startbelts, pos, screen, false);
            free(input);
        }
    }

    if (pos < 0 || pos > 4) {
        fprintf(stderr, "Error: illegal pos %d\n", pos);
        exit(EXIT_FAILURE);
    }

    while (play) {
        /*
        for (i = 0; i < 12; i++)
            printf("%s%d%c", (i == 0) ? "Deck is " : " ", deck[i], (i == 11) ? '\n' : ' ');
        */

        SDL_PollEvent(&ev);
        if (ev.type == SDL_KEYUP && ev.key.keysym.sym == SDLK_q)
            exit(EXIT_SUCCESS);
        else if (down && ev.type == SDL_MOUSEBUTTONUP && !g->villain[pos].dead) {
            opt = card_select(ev.button.x, ev.button.y, deck);

            if (opt != -1) {
                sdl_send_to(sock, "PLAY %d\n", deck[opt]);
                deck[opt] = 0;
            }
            down = 0;
        } else if (ev.type == SDL_MOUSEBUTTONDOWN) {
            down = 1;
        }

        if (SDLNet_CheckSockets(set, 0) && SDLNet_SocketReady(sock)) {
            input = sdl_receive_from(sock, debug);

            if (!strncmp(input, "TERMINATE", 9) || strstr(input, "\nTERMINATE"))
                play = false;

            parse_input(input, g, startbelts, pos, screen, true);

            free(input);
        }
    }
    
    puts("Game Over");

    do {
        SDL_WaitEvent(&ev);

        if (ev.type == SDL_KEYUP && ev.key.keysym.sym == SDLK_q)
            play = true;
    } while(!play);

    return EXIT_SUCCESS;
}

static void pre_render(SDL_Surface *s, char *name, double hs, double vs)
{
    draw_background(s, 0, 0);
    draw_hud(s, 0, 0);
    create_playerbox(s, name, hs * pbox_x, vs * pbox_y, 0, 0, false);
    set_wlevel(s, 0, 0, 0);
    set_points(s, 0, 0, 0);
}

static void parse_input(char *input, gamestr *g, int *startbelts, int pos, SDL_Surface *s, bool rend)
{
    char *p, b[BUFL];
    int i, needbelt = (startbelts[0] == 0);

    for (p = input; p != (char *)1; p = strchr(p, '\n') + 1) {
        needbelt = (startbelts[0] == 0);

        g->rings = g->points = false;
        strncpy(b, p, BUFL);
        parse_cmd(g, b);

        if (needbelt && g->rings) {
            for (i = 0; i < g->count; i++)
                startbelts[i] = g->villain[i].lifebelts;

            needbelt = false;
        }

        needbelt = g->points;
        if (needbelt)
            startbelts[0] = 0;

        if (rend) {
            render(s, g, pos, startbelts);
            SDL_UpdateRect(s, 0, 0, 0, 0);
        }
    }
}

/* vim: set sw=4 ts=4 et fdm=syntax: */
