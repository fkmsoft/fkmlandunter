#include "text_util.h"

struct tbox {
  char *s;
  SDL_Surface *screen;
  TTF_Font *font;
  unsigned x;
  unsigned y;
};

static SDL_Color font_fg = {0, 124, 0, 255};

Tbox create_textbox(SDL_Surface *s, TTF_Font *font, unsigned x, unsigned y)
{
  Tbox t = malloc(sizeof(struct tbox));
  t->s = 0;
  t->screen = s;
  t->font = font;
  t->x = x;
  t->y = y;

  return t;
}

void textbox_set(Tbox t, char *s)
{
  SDL_Rect r;
  SDL_Surface *txt;

  t->s = s;
  r.x = t->x;
  r.y = t->y;

  txt = TTF_RenderText_Blended(t->font, s, font_fg);
  SDL_BlitSurface(txt, 0, t->screen, &r);
}

char *textbox_get(Tbox t)
{
  return t->s;
}
