#include <stdlib.h>

#include "gui_util.h"
#include "text_util.h"

#define W 800
#define H 600
#define NAME "Guilord"

int main(int argc, char **argv)
{
    int i, k;
    char c = '0';
    double hs, vs;

    SDL_Surface *screen;
    SDL_Event event;
    Tbox t;

    char s[] = "hello\0\0\0\0\0\0\0\0\0\0\0";

    if (argc == 3) {
        screen = init_sdl(atoi(argv[1]), atoi(argv[2]));
        hs = atoi(argv[1]) / (float)W;
        vs = atoi(argv[2]) / (float)H;
    } else {
        screen = init_sdl(W, H);
        hs = vs = 1.0;
    }

    draw_hud(screen, 0, 0);
    create_playerbox(screen, NAME, hs * 628, vs * 398, 0, 10);

    for (i = 0; i < 12; i++)
       add_pcard(screen, 0, 0, i, i+1);

    set_wlevel(screen, 0, 0, 12);
    set_points(screen, 0, 0, -1);

    t = create_textbox(screen, getfont(), 36, 532);
    textbox_set(t, s);

    SDL_UpdateRect(screen, 0, 0, 0, 0);

    for (i = 9; i; i--) {
    	rm_lifebelt(screen, hs * 628, vs * 398, i);
	s[5 + c - '0'] = c;
	c++;
	textbox_set(t, s);
	k = (i % 3 + i % 5) % 12;
        add_pcard(screen, 0, 0, k, k + 1);

        SDL_UpdateRect(screen, 0, 0, 0, 0);
	SDL_Delay(1000);
    }

    do { SDL_PollEvent(&event); } while (event.type != SDL_KEYDOWN);

    return 0;
}

/* vim: set sw=4 ts=4 et fdm=syntax: */
