#ifndef FKML_TEXT_UTIL_H
#define FKML_TEXT_UTIL_H

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#define MAXLINES 512

extern double hstretch, vstretch;

typedef struct tbox *Tbox;
typedef struct chatbox *Chatbox;

Tbox create_textbox(SDL_Surface *s, TTF_Font *font, unsigned x, unsigned y);

void textbox_set(Tbox t, char *s);
char *textbox_get(Tbox t);
void textbox_update(Tbox t);

/* returns the corresponding character, or '\0' if not printable */
char getprintkey(SDLKey k, SDLMod m);

Chatbox create_chatbox(SDL_Surface *s, TTF_Font *font, unsigned x, unsigned y, int length);
void chatbox_append(Chatbox b, char *s);
void chatbox_render(Chatbox b);
void chatbox_scrollup(Chatbox b);
void chatbox_scrolldown(Chatbox b);

#endif /* FKML_TEXT_UTIL_H */
