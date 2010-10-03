#define _XOPEN_SOURCE 500

#include <stdlib.h>
#include <string.h>

#include "gui_util.h"
#include "text_util.h"

#define W 800
#define H 600
#define NAME "Guilord"
#define FONT "TOONISH.ttf"

int main(int argc, char **argv)
{
    int i, c, *w, down;
    gamestr g;
    player players[5];
    char names[5 * 9], *p;
    int belts[5];
    g.villain = players;

    SDL_Surface *screen;
    SDL_Event event;
    Tbox t;
    Chatbox out;

    if (argc == 3)
        screen = init_sdl(atoi(argv[1]), atoi(argv[2]), 0, FONT);
    else if (argc == 2)
        screen = init_sdl(W, H, argv[1], FONT);
    else
        screen = init_sdl(W, H, 0, FONT);

    for (i = 0; i < 5; i++) {
        sprintf(&names[9 * i], "Player %1d", i + 1);
        g.villain[i].name = &names[9 * i];
        g.villain[i].water_level = i;
        g.villain[i].points = i;
        g.villain[i].lifebelts = i;
        g.villain[i].played = i + 1;
        g.villain[i].dead = i == 4 ? 1 : 0;
        belts[i] = 6;
    }

    belts[0] = 10;

    for (i = 0; i < 12; i++)
        g.player.weathercards[i] = i + 1;

    g.w_card[0] = 5;
    g.w_card[1] = 12;

    t = create_textbox(screen, getfont(), 36, 532);
    out = create_chatbox(screen, getfont(), 36, 410, 10);

    g.count = 5;

    render(screen, &g, 0, belts);
    textbox_set(t, strdup(""));
    chatbox_append(out, strdup("foobar"));
    chatbox_append(out, strdup("baaz"));
    chatbox_append(out, strdup("quux"));
    chatbox_render(out);
    SDL_UpdateRect(screen, 0, 0, 0, 0);

    down = 0;
    for (i = 9; i; i--) {
        do {
            SDL_WaitEvent(&event);
            c = SDL_GetModState();

            if (event.type == SDL_QUIT)
                exit(0);
            if (event.type == SDL_KEYUP || event.type == SDL_MOUSEBUTTONUP)
                belts[0] = 0;
            if (event.type == SDL_KEYDOWN)
                down = 1;
        } while (belts[0]);
        belts[0] = 10;

        if (down && event.type == SDL_KEYUP) {
            if ((p = handle_keypress(event.key.keysym.sym, c, t, out))) {
                chatbox_append(out, p);
            }
#if 0
            if ((c = getprintkey(event.key.keysym.sym, SDL_GetModState()))) {
                /* append to message */
                p = malloc(strlen(textbox_get(t)) + 2);
                sprintf(p, "%s%c", textbox_get(t), c);

                /* fprintf(stderr, "text is now %s\n", p); */

                free(textbox_get(t));
                textbox_set(t, p);
            }

            switch(event.key.keysym.sym) {
            case SDLK_q:
                exit(0);
            case SDLK_PAGEUP:
                chatbox_scrollup(out);
                break;
            case SDLK_PAGEDOWN:
                chatbox_scrolldown(out);
                break;
            case SDLK_RETURN:
                chatbox_append(out, textbox_get(t));
                textbox_set(t, strdup(""));
                break;
            default:
                break;
            }
#endif /* 0 */
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
        textbox_update(t);
        chatbox_render(out);
        game_over(screen, &g, 0, 0, 0);

        SDL_UpdateRect(screen, 0, 0, 0, 0);
    }

    return 0;
}

/* vim: set sw=4 ts=4 et fdm=syntax: */
