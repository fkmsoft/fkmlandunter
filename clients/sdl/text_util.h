#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

typedef struct tbox *Tbox;

Tbox create_textbox(SDL_Surface *s, TTF_Font *font, unsigned x, unsigned y);

void textbox_set(Tbox t, char *s);
char *textbox_get(Tbox t);
