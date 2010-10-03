/* client.c
 *
 * the real sdl client
 *
 * (c) Fkmsoft, 2010
 */

/* this is for strdup() */
#define _XOPEN_SOURCE 500

#include "net_util.h"
#include "gui_util.h"
#include "text_util.h"
#include "config.h"

#define DEFPORT (1337)
#define DEFNAME ("SDL_Player")
#define DEFHOST ("127.0.0.1")
#define DEFRCFILE ("~/.fkmlandunterrc")

#define DEBUG 0

static const int W = 800;
static const int H = 600;

struct net_s {
    SDLNet_SocketSet set;
    TCPsocket sock;
    bool *play;
    bool *push;
};

static bool parse_input(char *input, gamestr *g, int *startbelts, int pos, SDL_Surface *s, bool needbelt, Chatbox chat);
static int network_thread(struct net_s *data);
static void chat_append(Chatbox chat, char *name, char *msg);

int main(int argc, char **argv)
{
    bool  netpush = true,
          play = true,
          needbelt = true;
    char *input,
         *p,
          buf[BUFL];
    int   card,
          pos,
          i,
          startbelts[5],
          *deck,
          mdown = 0,
          kdown = 0;

    gamestr        *g;
    struct config_s conf;
    struct net_s    netdata;

    SDL_Surface     *screen;
    SDL_Thread      *net;
    SDL_Event        ev;
    TCPsocket        sock;
    SDLNet_SocketSet set;
    Tbox             chat_input;
    Chatbox          chat;

    conf.name  = DEFNAME;
    conf.host  = DEFHOST;
    conf.port  = DEFPORT;
    conf.x_res = W;
    conf.y_res = H;
    conf.debug = false;

    p = DEFRCFILE;

    for (i = 0; i < argc; i++)
        if (argv[i][0] == '-' && argv[i][1] == 'f' && argv[i][2] == 0) {
            p = argv[i+1];
            break;
        }

    config_fromfile(p, &conf);

    config_fromargv(argc, argv, &conf);

    screen     = init_sdl(conf.x_res, conf.y_res, conf.datadir, conf.font);
    chat_input = create_textbox(screen, getfont(), 36, 532);
    chat       = create_chatbox(screen, getfont(), 36, 410, 10);

    pre_render(screen, conf.name);

    p = malloc(1); /* these two lines are `strdup("")' */
    *p = 0;
    textbox_set(chat_input, p);

    chatbox_render(chat);
    SDL_UpdateRect(screen, 0, 0, 0, 0);

    g = create_game();
    g->player.name = conf.name;

    IPaddress addr = compute_address(conf.host, conf.port);

    sock = SDLNet_TCP_Open(&addr);
    if (sock == NULL) {
        printf("cannot connect to %s:%d\n", conf.host, conf.port);
        exit(EXIT_FAILURE);
    }

    sdl_send_to(sock, "LOGIN %s\n", conf.name);

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
#define PREREND0R \
pre_render(screen, conf.name); \
textbox_update(chat_input); \
chatbox_render(chat); \
SDL_UpdateRect(screen, 0, 0, 0, 0)

        SDL_PollEvent(&ev);
        card = SDL_GetModState();

        if (ev.type == SDL_QUIT) {
            sdl_send_to(sock, "LOGOUT bye\n");
            SDLNet_TCP_Close(sock);
            exit(EXIT_SUCCESS);
        } else if (kdown && ev.type == SDL_KEYUP) {
            if ((p = handle_keypress(ev.key.keysym.sym, card, chat_input, chat))) {
                chat_append(chat, conf.name, p);
                sdl_send_to(sock, "MSG %s\n", p);
            }
            kdown = 0;
            PREREND0R;
        } else if (ev.type == SDL_KEYDOWN) {
            kdown = 1;
        }

        if (SDLNet_CheckSockets(set, 200) && SDLNet_SocketReady(sock)) {
            input = sdl_receive_from(sock, conf.debug);
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
                needbelt = parse_input(input, g, startbelts, pos, screen, needbelt, chat);
            else
                needbelt = parse_input(input, g, startbelts, pos, screen, needbelt, chat);

            free(input);
        }
    }

#define REND0R \
render(screen, g, pos, startbelts); \
textbox_update(chat_input); \
chatbox_render(chat); \
SDL_UpdateRect(screen, 0, 0, 0, 0)

    REND0R;

    if (pos < 0 || pos > 4) {
        fprintf(stderr, "Error: illegal pos %d\n", pos);
        exit(EXIT_FAILURE);
    }

    netdata.sock = sock;
    netdata.set = set;
    netdata.play = &play;
    netdata.push = &netpush;
    net = SDL_CreateThread((int (*)(void *))network_thread, &netdata);

    while (play) {

        SDL_WaitEvent(&ev);
        card = SDL_GetModState();

        if (ev.type == SDL_QUIT) {
            sdl_send_to(sock, "LOGOUT bye\n");
            SDLNet_TCP_Close(sock);
            SDL_KillThread(net);
            exit(EXIT_SUCCESS);
        } else if (mdown && ev.type == SDL_MOUSEBUTTONUP && !g->villain[pos].dead) {
            card = card_select(ev.button.x, ev.button.y, deck);

            if (card != -1) {
                sdl_send_to(sock, "PLAY %d\n", deck[card]);
                deck[card] = 0;
            }
            mdown = 0;
            REND0R;
        } else if (ev.type == SDL_MOUSEBUTTONDOWN) {
            mdown = 1;
        } else if (ev.type == SDL_KEYDOWN) {
            kdown = 1;
        } else if (ev.type == SDL_USEREVENT) {
            input = sdl_receive_from(sock, conf.debug);

            if (!strncmp(input, "TERMINATE", 9) || strstr(input, "\nTERMINATE"))
                play = false;

            needbelt = parse_input(input, g, startbelts, pos, screen, needbelt, chat);
            free(input);
            netpush = true;
            REND0R;
        } else if (ev.type == SDL_KEYUP) {
            kdown = 0;
            handle_keypress(ev.key.keysym.sym, card, chat_input, chat);
            REND0R;
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

static bool parse_input(char *input, gamestr *g, int *startbelts, int pos, SDL_Surface *s, bool needbelt, Chatbox chat)
{
    char *p, b[BUFL];
    int i;

    for (p = input; p != (char *)1; p = strchr(p, '\n') + 1) {
        g->rings = g->points = false;
        strncpy(b, p, BUFL);
        if (parse_cmd(g, b) == 1)
            chat_append(chat, strchr(p, ' ') + 1, 0);

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

/* magic argument msg: if NULL, disassemble name */
static void chat_append(Chatbox chat, char *name, char *msg)
{
    char *p, *q;

    if (msg) {
        p = malloc(strlen(msg) + strlen(name) + 4);
        sprintf(p, "<%s> %s", name, msg);
    } else {
        p = malloc(strlen(name) + 3);
        q = strchr(name, ' ');
        *q = 0;
        sprintf(p, "<%s> %s", name, q+1);
    }

    chatbox_append(chat, p);
}

/* vim: set sw=4 ts=4 et fdm=syntax: */
