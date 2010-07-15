#include <stdlib.h>

#include "gui_util.h"
#include "text_util.h"

#define W 800
#define H 600

int main(int argc, char **argv)
{
    int i;
    char c = '0';

    SDL_Surface *screen;
    SDL_Event event;
    Tbox t;

    char s[] = "hello\0\0\0\0\0\0\0\0\0\0\0";

    if (argc == 3)
        screen = init_sdl(atoi(argv[1]), atoi(argv[2]));
    else
        screen = init_sdl(W, H);

    create_playerbox(screen, 0, 0, 0, 10);

    for (i = 0; i < 12; i++)
       add_pcard(screen, 0, 0, i, i+1);

    set_wlevel(screen, 0, 0, 12);
    set_points(screen, 0, 0, -1);

    t = create_textbox(screen, getfont(), 36, 532);
    textbox_set(t, s);

    SDL_UpdateRect(screen, 0, 0, 0, 0);

    for (i = 9; i; i--) {
    	rm_lifebelt(screen, 0, 0, i);
	s[5 + c - '0'] = c;
	c++;
	textbox_set(t, s);

        SDL_UpdateRect(screen, 0, 0, 0, 0);
	SDL_Delay(1000);
    }

    do { SDL_PollEvent(&event); } while (event.type != SDL_KEYDOWN);

    kill_lifebelts(screen, 0, 0);
    SDL_UpdateRect(screen, 0, 0, 0, 0);
    SDL_Delay(1000);

    return 0;
}
