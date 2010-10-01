#include "text_util.h"

#define TEXT_UTIL_DEBUG 0

struct tbox {
    char *s;
    SDL_Surface *screen;
    TTF_Font *font;
    unsigned x;
    unsigned y;
};

struct chatbox {
    char *lines[MAXLINES];  /* text storage */
    int   first;            /* first line displayed */
    int   length;           /* max number of lines displayed */
    int   last;             /* last line used */
    int   fontsize;         /* height of font in pixels */
    int   fill;             /* how many lines do we have */
    Tbox  output;           /* text box used for printing */
};

static SDL_Color font_fg = {255, 0, 0, 255};

Tbox create_textbox(SDL_Surface *s, TTF_Font *font, unsigned x, unsigned y)
{
    Tbox t = malloc(sizeof(struct tbox));
    t->s = 0;
    t->screen = s;
    t->font = font;
    t->x = hstretch * x;
    t->y = vstretch * y;

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
    if (TEXT_UTIL_DEBUG > 1)
        fprintf(stderr, "Rendering textbox at %d/%d\n", r.x, r.y);
}

char *textbox_get(Tbox t)
{
    return t->s;
}

void textbox_update(Tbox t)
{
    textbox_set(t, t->s);
}

char getprintkey(SDLKey k, SDLMod m)
{
    char c = 0;

    if (k > 31 && k < 127)
        c = k;

    /* fprintf(stderr, "Mod is %d\n", m); */
    if (m & KMOD_SHIFT) {
        if (k > 96 && k < 123)
            c -= 32;
        else if (k > 48 && k < 58)
            c -= 16;
    }

    return c;
}

Chatbox create_chatbox(SDL_Surface *s, TTF_Font *font, unsigned x, unsigned y, int length)
{
    int i;

    Chatbox b = malloc(sizeof(struct chatbox));
    b->first = b->last = -1;
    b->length = length;
    b->output = create_textbox(s, font, x, y);

    b->fontsize = 10;

    for (i = 0; i < MAXLINES; i++)
        b->lines[i] = 0;

    return b;
}

/* NOTICE: The string s must be freeable, and will be freed whenever chatbox
 *         thinks the time is right. */
void chatbox_append(Chatbox b, char *s)
{
    /*
    int i;

    if (b->last + 1 >= MAXLINES) {
        fprintf(stderr, "Chatbox full, doing complete reset\n");
        for (i = 0; i < MAXLINES; i++) {
            free(b->lines[i]);
            b->lines[i] = 0;
        }
        b->first = b->last = -1;
    }
    */

    if (b->fill < MAXLINES)
        b->fill++;

    b->last = (b->last + 1 ) % MAXLINES;

    if (b->lines[b->last]) {
        if (TEXT_UTIL_DEBUG)
            fprintf(stderr, "Removed `%s' from history\n", b->lines[b->last]);
        free(b->lines[b->last]);
    }

    b->lines[b->last] = s;
    if (TEXT_UTIL_DEBUG)
        fprintf(stderr, "Added string `%s' in slot %d\n", b->lines[b->last], b->last);
}

void chatbox_render(Chatbox b)
{
    int i, y, last;

    if (b->last == -1) return;

    y = b->output->y;
    font_fg.r = 0;

    if (b->fill < b->length) {
        i = 0;
        last = b->last;
    } else if (b->first == -1) {
        i = (MAXLINES + b->last - b->length + 1) % MAXLINES;
        last = b->last;
    } else {
        i = b->first;
        if (i < b->last) { /* last line does NOT wrap */
            last = i + b->length - 1;
            if (last > b->last)
                last = b->last;
        } else { /* last line wraps */
            last = (MAXLINES + i + b->length - 1) % MAXLINES;
            if (last < i /* wraps */ && last > b->last)
                last = b->last;
        }
    }

    if (TEXT_UTIL_DEBUG)
        fprintf(stderr, "rendering slots %d - %d\n", i, last);
    for (i = i < 0 ? 0 : i; i != last; i = (i + 1) % MAXLINES) {
        textbox_set(b->output, b->lines[i]);
        b->output->y += b->fontsize;
    }
    textbox_set(b->output, b->lines[i]);

    b->output->y = y;
    font_fg.r = 255;
}

void chatbox_scrollup(Chatbox b)
{
    if (b->first == -1)
        b->first = b->last - b->length;
    else
        b->first -= b->length;

    if (b->first < 0)
        b->first = 0;
}

void chatbox_scrolldown(Chatbox b)
{
    if (b->first != -1) {
        b->first += b->length;

        if(b->first >= b->last)
            b->first = -1;
    }
}

/* vim: set sw=4 ts=4 et fdm=syntax: */
