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

struct net_s {
    SDLNet_SocketSet set;
    TCPsocket sock;
    bool *play;
    bool *push;
};

static bool parse_input(char *input, gamestr *g, int *startbelts, int pos, SDL_Surface *s, bool needbelt);
static int network_thread(struct net_s *data);

int main(int argc, char **argv)
{
    bool debug = false, netpush = true;
    bool play = true, needbelt = true;
    char *name = DEFNAME, *host = DEFHOST;
    char *input, *p, buf[BUFL];
    int opt, port = DEFPORT, w = W, h = H, pos, i;
    int startbelts[5], *deck;
    int mdown = 0, kdown = 0;
    gamestr *g;
    SDL_Surface *screen;
    SDL_Thread *net;
    SDL_Event ev;
    TCPsocket sock;
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

    screen = init_sdl(w, h);

    pre_render(screen, name);
    SDL_UpdateRect(screen, 0, 0, 0, 0);

    g = create_game();
    g->player.name = name;

    IPaddress addr = compute_address(host, port);

    sock = SDLNet_TCP_Open(&addr);
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
        pre_render(screen, name);
        SDL_UpdateRect(screen, 0, 0, 0, 0);

        SDL_PollEvent(&ev);
        if (ev.type == SDL_QUIT ||
            (ev.type == SDL_KEYUP && ev.key.keysym.sym == SDLK_q)) {
            sdl_send_to(sock, "LOGOUT bye\n");
            SDLNet_TCP_Close(sock);
            exit(EXIT_SUCCESS);
        }

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

            if (play)
                needbelt = parse_input(input, g, startbelts, pos, screen, needbelt);
            else
                needbelt = parse_input(input, g, startbelts, pos, screen, needbelt);

            free(input);
        }
    }

    render(screen, g, pos, startbelts);
    SDL_UpdateRect(screen, 0, 0, 0, 0);

    if (pos < 0 || pos > 4) {
        fprintf(stderr, "Error: illegal pos %d\n", pos);
        exit(EXIT_FAILURE);
    }

    struct net_s netdata;
    netdata.sock = sock;
    netdata.set = set;
    netdata.play = &play;
    netdata.push = &netpush;
    net = SDL_CreateThread((int (*)(void *))network_thread, &netdata);

    while (play) {
        SDL_WaitEvent(&ev);

        if (ev.type == SDL_QUIT ||
            (kdown && ev.type == SDL_KEYUP && ev.key.keysym.sym == SDLK_q)) {
            sdl_send_to(sock, "LOGOUT bye\n");
            SDLNet_TCP_Close(sock);
            SDL_KillThread(net);
            exit(EXIT_SUCCESS);
        } else if (mdown && ev.type == SDL_MOUSEBUTTONUP && !g->villain[pos].dead) {
            opt = card_select(ev.button.x, ev.button.y, deck);

            if (opt != -1) {
                sdl_send_to(sock, "PLAY %d\n", deck[opt]);
                deck[opt] = 0;

                render(screen, g, pos, startbelts);
                SDL_UpdateRect(screen, 0, 0, 0, 0);
            }
            mdown = 0;
        } else if (ev.type == SDL_MOUSEBUTTONDOWN) {
            mdown = 1;
        } else if (ev.type == SDL_KEYDOWN) {
            kdown = 1;
        } else if (ev.type == SDL_USEREVENT) {
            input = sdl_receive_from(sock, debug);

            if (!strncmp(input, "TERMINATE", 9) || strstr(input, "\nTERMINATE"))
                play = false;

            needbelt = parse_input(input, g, startbelts, pos, screen, needbelt);
            free(input);
            netpush = true;

            render(screen, g, pos, startbelts);
            SDL_UpdateRect(screen, 0, 0, 0, 0);
        }
    }

    SDL_WaitThread(net, 0);
    
    game_over(screen, g, pos, 0, 0);
    SDL_UpdateRect(screen, 0, 0, 0, 0);

    do {
        SDL_WaitEvent(&ev);

        if (ev.type == SDL_KEYUP && ev.key.keysym.sym == SDLK_q)
            play = true;
    } while(!play);

    return EXIT_SUCCESS;
}

static bool parse_input(char *input, gamestr *g, int *startbelts, int pos, SDL_Surface *s, bool needbelt)
{
    char *p, b[BUFL];
    int i;

    for (p = input; p != (char *)1; p = strchr(p, '\n') + 1) {
        g->rings = g->points = false;
        strncpy(b, p, BUFL);
        parse_cmd(g, b);

        if (needbelt && g->rings) {
            for (i = 0; i < g->count; i++)
                startbelts[i] = g->villain[i].lifebelts;

            needbelt = false;
        } else if (!needbelt) {
            needbelt = g->points;
        }
    }

    return needbelt;
}

static int network_thread(struct net_s *data)
{
    SDLNet_SocketSet set = data->set;
    TCPsocket sock = data->sock;
    bool *play = data->play;
    bool *push = data->push;
    SDL_Event net_event;

    net_event.type = SDL_USEREVENT;
    net_event.user.code = 0;
    net_event.user.data1 = 0;
    net_event.user.data2 = 0;

    while (*play)
        if (*push && SDLNet_CheckSockets(set, -1) && SDLNet_SocketReady(sock)) {
            SDL_PushEvent(&net_event);
            *push = false;
        }

    return 0;
}

/* vim: set sw=4 ts=4 et fdm=syntax: */
