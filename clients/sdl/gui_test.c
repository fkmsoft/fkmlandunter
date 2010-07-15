#include "gui_util.h"

#define W 1600
#define H 1200

int main()
{
    int i;

    SDL_Surface *screen;
    SDL_Event event;

    screen = init_sdl(W, H);

    create_playerbox(screen, 0, 0, 0, 10);
    set_wlevel(screen, 0, 0, 12);
    set_points(screen, 0, 0, -1);

    SDL_UpdateRect(screen, 0, 0, 0, 0);

    for (i = 9; i; i--) {
    	rm_lifebelt(screen, 0, 0, i);
	printf("%d lifebelts\n", i);
        SDL_UpdateRect(screen, 0, 0, 0, 0);
	SDL_Delay(1000);
    }

    do { SDL_PollEvent(&event); } while (event.type != SDL_KEYDOWN);

    kill_lifebelts(screen, 0, 0);
    SDL_UpdateRect(screen, 0, 0, 0, 0);
    SDL_Delay(1000);

    return 0;
}
