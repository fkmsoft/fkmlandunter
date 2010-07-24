#include <stdlib.h>

#include "gui_util.h"
#include "text_util.h"

#define W 800
#define H 600
#define NAME "Guilord"

int main(int argc, char **argv)
{
    int i, c, *w;
    double hs, vs;
    gamestr g;
    player players[5];
    char names[5 * 9];
    int belts[5];
    g.villain = players;

    SDL_Surface *screen;
    SDL_Event event;
    Tbox t;

    char s[] = "hello_\0\0\0\0\0\0\0\0\0\0\0";

    if (argc == 3) {
        screen = init_sdl(atoi(argv[1]), atoi(argv[2]));
        hs = atoi(argv[1]) / (float)W;
        vs = atoi(argv[2]) / (float)H;
    } else {
        screen = init_sdl(W, H);
        hs = vs = 1.0;
    }

    for (i = 0; i < 5; i++) {
        sprintf(&names[9 * i], "Player %1d", i + 1);
        g.villain[i].name = &names[9 * i];
        g.villain[i].water_level = i;
        g.villain[i].points = i;
        g.villain[i].lifebelts = i;
        g.villain[i].played = i + 1;
        g.villain[i].dead = i;
        belts[i] = 6;
    }

    belts[0] = 10;

    for (i = 0; i < 12; i++)
        g.player.weathercards[i] = i + 1;

    g.w_card[0] = 5;
    g.w_card[1] = 12;

    t = create_textbox(screen, getfont(), hs * 36, vs * 532);

    g.count = 5;

    render(screen, &g, 0, belts);
    textbox_set(t, s);
    SDL_UpdateRect(screen, 0, 0, 0, 0);

    for (i = 9; i; i--) {
        do {
            SDL_WaitEvent(&event);

            if (event.type == SDL_QUIT)
                exit(0);
            if (event.type == SDL_KEYUP || event.type == SDL_MOUSEBUTTONUP)
                belts[0] = 0;
        } while (belts[0]);
        belts[0] = 10;

        if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_q) {
            exit(0);
        } else if (event.type == SDL_MOUSEBUTTONUP) {
            w = g.player.weathercards;

            c = card_select(event.button.x, event.button.y, w);

            if (c != -1) {
                g.villain[0].played = w[c];
                w[c] = 0;
            }
        }

    	g.villain[0].lifebelts = i;
        if (i == 1) i = 2;

        render(screen, &g, 0, belts);
        s[15 - i] = '0' + i;
        textbox_set(t, s);
        game_over(screen, &g, 0, 0, 0);

        SDL_UpdateRect(screen, 0, 0, 0, 0);
    }

    return 0;
}

/* vim: set sw=4 ts=4 et fdm=syntax: */
