#ifndef FKML_TEXT_UTIL_H
#define FKML_TEXT_UTIL_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define MAXLINES 512

extern double hstretch, vstretch;

typedef struct tbox *Tbox;
typedef struct chatbox *Chatbox;

Tbox create_textbox(SDL_Renderer *s, TTF_Font *font, unsigned x, unsigned y);

void textbox_set(Tbox t, char *s);
char *textbox_get(Tbox t);
void textbox_update(Tbox t);

/* returns the corresponding character, or '\0' if not printable */
char getprintkey(SDL_Keycode k, SDL_Keymod m);

/* returns 0 or pointer to message */
char *handle_keypress(SDL_Keycode k, SDL_Keymod m, Tbox in, Chatbox out);

Chatbox create_chatbox(SDL_Renderer *s, TTF_Font *font, unsigned x, unsigned y, int length);
void chatbox_append(Chatbox b, char *s);
void chatbox_render(Chatbox b);

/* these went static
void chatbox_scrollup(Chatbox b);
void chatbox_scrolldown(Chatbox b);
*/

#endif /* FKML_TEXT_UTIL_H */
