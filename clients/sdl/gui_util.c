#include "gui_util.h"

#define FONTSIZE (24)

static SDL_Surface *act_border, *pas_border, *name_border,
    *box_lifebelt, *act_lifebelt, *pas_lifebelt,
    *pointsbox, *wlevelbox, *defava;

static TTF_Font *font;
static SDL_Color font_fg = {0, 0, 0, 255};

SDL_Surface *init_sdl(int w, int h)
{
    int ckey;
    SDL_Surface *screen, *temp;

#define LOAD(a, b) if ((temp = IMG_Load(a))) \
    { b = SDL_DisplayFormat(temp); SDL_FreeSurface(temp); } \
    else { fprintf(stderr, "Could not load file >>%s<<: %s\n", a, IMG_GetError()); exit(EXIT_FAILURE); }

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        exit(EXIT_FAILURE);
    }

    if (!(screen = SDL_SetVideoMode(w, h, 0, 0))) {
        exit(EXIT_FAILURE);
    }

    if (TTF_Init() == -1) {
        exit(EXIT_FAILURE);
    }

    atexit(SDL_Quit);

    /* load all images */
    LOAD(ACT_BORDER, act_border)
    LOAD(PAS_BORDER, pas_border)
    LOAD(NAME_BORDER, name_border)
    LOAD(BOX_LIFEBELT, box_lifebelt)
    LOAD(ACT_LIFEBELT, act_lifebelt)
    LOAD(PAS_LIFEBELT, pas_lifebelt)
    LOAD(POINTSBOX, pointsbox)
    LOAD(WLEVELBOX, wlevelbox)
    LOAD(DEFAVA, defava)

    /* load font */
    if(!(font = TTF_OpenFont(DEFFONT, FONTSIZE))) {
        fprintf(stderr, "Could not load file >>%s<<: %s\n", DEFFONT, TTF_GetError());
	exit(EXIT_FAILURE);
    };
    /*cache_glyphs();*/

    ckey = SDL_MapRGB(screen->format, 0, 0, 0);
    SDL_SetColorKey(act_lifebelt, SDL_SRCCOLORKEY | SDL_RLEACCEL, ckey);
    SDL_SetColorKey(pas_lifebelt, SDL_SRCCOLORKEY | SDL_RLEACCEL, ckey);

    return screen;
}

void create_playerbox(SDL_Surface *s, unsigned x, unsigned y, char *avatar, unsigned lifebelts)
{
    int i, stretch = 2;
    SDL_Rect r;
    SDL_Surface *txt;

    /* draw border */
    r.x = x;
    r.y = y;
    SDL_BlitSurface(act_border, 0, s, &r);

    r.x += stretch * 4;
    r.y += stretch * 4;
    if (!avatar)
        SDL_BlitSurface(defava, 0, s, &r);
    else {
        /* FIXME: load real ava */
    }

    r.x = x + stretch * 77;
    SDL_BlitSurface(wlevelbox, 0, s, &r);

    txt = TTF_RenderText_Blended(font, "Wasser", font_fg);
    r.x += stretch * 3;
    r.y += stretch * 34;
    SDL_BlitSurface(txt, 0, s, &r);
    r.x -= stretch * 3;
    r.y -= stretch * 34;

    r.y = y + stretch * 55;
    SDL_BlitSurface(pointsbox, 0, s, &r);

    txt = TTF_RenderText_Blended(font, "Punkte", font_fg);
    r.x += stretch * 4;
    r.y += stretch * 34;
    SDL_BlitSurface(txt, 0, s, &r);
    r.x -= stretch * 4;
    r.y -= stretch * 34;

    r.x = x + stretch * 4;
    r.y = y + stretch * 85;
    SDL_BlitSurface(name_border, 0, s, &r);

    txt = TTF_RenderText_Blended(font, "Guilord", font_fg);
    r.x += stretch * 2;
    r.y += stretch * 4;
    SDL_BlitSurface(txt, 0, s, &r);
    r.x -= stretch * 2;
    r.y -= stretch * 4;

    r.x += stretch;
    r.y = y + stretch * 105;
    SDL_BlitSurface(box_lifebelt, 0, s, &r);

    /* draw lifebelts */
    /* set_lifebelts(s, x, y, lifebelts, lifebelts); */
    for (i = 0; i < lifebelts; i++)
       add_lifebelt(s, x, y, i);
}

int set_lifebelts(SDL_Surface *s, unsigned x, unsigned y, unsigned n, unsigned max)
{
    int i, j, stretch = 2;
    SDL_Rect r;

    /* draw lifebelts */
    r.x = x + stretch * 5;
    r.y = y + stretch * 106;
    for (i = j = 0; i < max; j = ++i) {
    #define BELTSIZE 24
        if (i == 5) {
	    r.x -= stretch * 4 * BELTSIZE;
	    r.y += stretch * BELTSIZE;
	} else if (i > 0) {
	    r.x += stretch * BELTSIZE;
	}
	if (j >= n)
	    SDL_BlitSurface(pas_lifebelt, 0, s, &r);
	else
	    SDL_BlitSurface(act_lifebelt, 0, s, &r);
    }

    printf("Drawing %d/%d lifebelts done\n", n, max);

    return n;
}

int kill_lifebelts(SDL_Surface *s, unsigned x, unsigned y)
{
    int stretch = 2;
    SDL_Rect r;

    r.x = x + 4 * stretch;
    r.y = y + stretch * 105;
    SDL_BlitSurface(box_lifebelt, 0, s, &r);

    return 1;
}

int add_lifebelt(SDL_Surface *s, unsigned x, unsigned y, unsigned n)
{
    int i, j, stretch = 2;
    SDL_Rect r;

    r.x = x + stretch * 5;
    r.y = y + stretch * 106;

    r.x += stretch * (n % 5) * BELTSIZE;

    if (n >= 5)
        r.y += stretch * BELTSIZE;
    SDL_BlitSurface(act_lifebelt, 0, s, &r);

    return 1;
}

int rm_lifebelt(SDL_Surface *s, unsigned x, unsigned y, unsigned n)
{
    int i, j, stretch = 2;
    SDL_Rect r;

    r.x = x + stretch * 5;
    r.y = y + stretch * 106;

    r.x += stretch * (n % 5) * BELTSIZE;

    if (n >= 5)
        r.y += stretch * BELTSIZE;

    SDL_BlitSurface(pas_lifebelt, 0, s, &r);

    return 1;
}

int set_wlevel(SDL_Surface *s, unsigned x, unsigned y, unsigned n)
{
    SDL_Rect r;
    SDL_Surface *txt;
    char buf[5];
    int stretch = 2;

    snprintf(buf, 5, "%i", n);
    puts(buf);
    txt = TTF_RenderText_Blended(font, buf, font_fg);
    if (!txt) puts("?");

    r.x = stretch * 94;
    r.y = stretch * 18;
    SDL_BlitSurface(txt, 0, s, &r);

    return n;
}

int set_points(SDL_Surface *s, unsigned x, unsigned y, int n)
{
    SDL_Rect r;
    SDL_Surface *txt;
    char buf[5];
    int stretch = 2;

    snprintf(buf, 5, "%i", n);
    puts(buf);
    txt = TTF_RenderText_Blended(font, buf, font_fg);
    if (!txt) puts("?");
    r.x = stretch * 94;
    r.y = stretch * 66;
    SDL_BlitSurface(txt, 0, s, &r);

    return n;
}

TTF_Font *getfont(void)
{
  return font;
}
