#include "text_util.h"

#define TEXT_UTIL_DEBUG 0
#define FONT_PIXELSIZE 10

struct tbox {
    char *s;
    SDL_Surface *screen;
    TTF_Font *font;
    unsigned x;
    unsigned y;
};

struct chatbox {
    char *lines[MAXLINES];  /* text storage */
    int   upper;            /* upper line displayed, -1 means from end */
    int   length;           /* max number of lines displayed */
    int   first;            /* first line used */
    int   last;             /* last line used */
    int   fontsize;         /* height of font in pixels */
    Tbox  output;           /* text box used for printing */
};

static void chatbox_scrollup(Chatbox b);
static void chatbox_scrolldown(Chatbox b);
/* static void chatbox_scrollend(Chatbox b); */

static SDL_Color font_fg = {0, 0, 0, 255};

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
        fprintf(stderr, "%p Rendering textbox at %d/%d: `%s'\n",
            t->screen, r.x, r.y, s);
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

    if (TEXT_UTIL_DEBUG)
        fprintf(stderr, "Got `%s'\n", SDL_GetKeyName(k));

    if (m & KMOD_CTRL
        || m & KMOD_ALT
        || m & KMOD_META)
        return 0;

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

/* returns 0 or pointer to message */
char *handle_keypress(SDLKey k, SDLMod m, Tbox in, Chatbox out)
{
    int i;
    char *p, *q;

    if ((i = getprintkey(k, m))) {
        /* append to message */
        p = malloc(strlen(textbox_get(in)) + 2);
        sprintf(p, "%s%c", textbox_get(in), i);

        if (TEXT_UTIL_DEBUG)
            fprintf(stderr, "text is now %s\n", p);

        free(textbox_get(in));
        if (TEXT_UTIL_DEBUG)
            fprintf(stderr, "by handler routine: ");
        textbox_set(in, p);
    }

    switch(k) {
    case SDLK_q:
        if (m & KMOD_CTRL) {
            fprintf(stderr, "Quit by ^Q\n");
            exit(0);
        }
        break;
    case SDLK_PAGEUP:
        chatbox_scrollup(out);
        break;
    case SDLK_PAGEDOWN:
        chatbox_scrolldown(out);
        break;
    case SDLK_KP_ENTER:
    case SDLK_RETURN:
        if (strcmp("", p = textbox_get(in))) {
            if (TEXT_UTIL_DEBUG > 1)
                fprintf(stderr, "by handler routine: ");

            q = malloc(1); /* these two lines are `strdup("")' */
            *q = 0;
            textbox_set(in, q);

            /* chatbox_scrollend(out); */
            out->upper = -1;

            return p;
        }
        break;
    case SDLK_BACKSPACE:
        if (strcmp("", p = textbox_get(in))) {
            if (TEXT_UTIL_DEBUG)
                fprintf(stderr, "backspace\n");

            while(*p)
                p++;
            *(p-1) = 0;
        }
        break;
    default:
        /* fprintf(stderr, "FIXME: fell down to default case in %s:%d\n", __FILE__, __LINE__); */
        break;
    }

    return 0;
}

Chatbox create_chatbox(SDL_Surface *s, TTF_Font *font, unsigned x, unsigned y, int length)
{
    int i;

    Chatbox b = malloc(sizeof(struct chatbox));
    b->first = b->upper = b->last = -1;
    b->length = length;
    b->output = create_textbox(s, font, x, y);

    b->fontsize = FONT_PIXELSIZE * vstretch;

    for (i = 0; i < MAXLINES; i++)
        b->lines[i] = 0;

    return b;
}

/* NOTICE: The string s must be freeable, and will be freed whenever chatbox
 *         thinks the time is right. */
void chatbox_append(Chatbox b, char *s)
{
    b->last = (MAXLINES + b->last + 1) % MAXLINES;

    if (b->last == b->first) {
        if (TEXT_UTIL_DEBUG)
            fprintf(stderr, "Removed `%s' from history\n", b->lines[b->last]);
        free(b->lines[b->last]);

        b->first = (MAXLINES + b->last + 1) % MAXLINES;
    }

    if (b->first == -1)
        b->first = 0;

    b->lines[b->last] = s;
    if (TEXT_UTIL_DEBUG)
        fprintf(stderr, "Added string `%s' in slot %d\n", b->lines[b->last], b->last);
}

void chatbox_render(Chatbox b)
{
    int i, y, last;

    if (TEXT_UTIL_DEBUG)
        fprintf(stderr, "Rendering chatbox at %d/%d\n", b->output->x, b->output->y);

    if (b->first == -1) return;

    y = b->output->y;

    if ((MAXLINES - b->first + b->last) % MAXLINES < b->length) {
        i = b->first;
        last = b->last;
    } else if (b->upper != -1) {
        i = b->upper;
        last = (MAXLINES + b->upper + b->length - 1) % MAXLINES;
    } else {
        i = (MAXLINES + b->last - b->length + 1) % MAXLINES;
        last = b->last;
    }

    if (TEXT_UTIL_DEBUG)
        fprintf(stderr, "rendering slots %d - %d (%d)\n", i, last, b->length);

    for (; i != last; i = (i + 1) % MAXLINES) {
        textbox_set(b->output, b->lines[i]);
        b->output->y += b->fontsize;
        if (TEXT_UTIL_DEBUG)
            fprintf(stderr, "line %d: %s\n", i, b->lines[i]);
    }
    if (b->lines[i]) {
        textbox_set(b->output, b->lines[i]);
        if (TEXT_UTIL_DEBUG)
            fprintf(stderr, "line %d: %s\n", i, b->lines[i]);
    }

    b->output->y = y;
}

static void chatbox_scrollup(Chatbox b)
{
    int i;

    if (b->upper == -1)
        b->upper = b->last;

    for (i = 0; b->upper != b->first && i < b->length; i++)
        b->upper = (MAXLINES + b->upper - 1) % MAXLINES;

    if (TEXT_UTIL_DEBUG)
        fprintf(stderr, "scrolling up to %d\n", b->upper);
}

static void chatbox_scrolldown(Chatbox b)
{
    int i, last;

    last = (MAXLINES + b->last - b->length + 1) % MAXLINES;

    if (b->upper != -1) {
        for (i = 0; b->upper != last && i < b->length; i++)
            b->upper = (b->upper + 1) % MAXLINES;

        if (b->upper == b->last)
            b->upper = -1;
    }

    if (TEXT_UTIL_DEBUG)
        fprintf(stderr, "scrolling down to %d\n", b->upper);
}

/* vim: set sw=4 ts=4 et fdm=syntax: */
