#include "gui_util.h"

#define FONTSIZE (11)
#define FONTSIZE2 (15)
#define FONTSIZE3 (36)

#define BUFLEN 1024
#define DEBUG 0

static SDL_Surface  
    *act_lifebelt, *pas_lifebelt, *act_border, *drownava,
    *defava, *avabox, *hud, *pcard, *wcard, *table;

static TTF_Font *font, *font2, *font3;
static SDL_Color font_fg = {0, 0, 0, 255};
/*static SDL_Color font_fg2 = {255, 255, 255, 255};*/

double hstretch, vstretch;

const int pbox_x = 623, pbox_y = 393;

SDL_Surface *init_sdl(int w, int h)
{
    int ckey;
    char buf[BUFLEN];
    SDL_Surface *screen, *temp;
    struct stat s_stat;

    snprintf(buf, BUFLEN, "%s%d_%d", DATADIR, w, h);
    if (stat(buf, &s_stat) == -1 || !S_ISDIR(s_stat.st_mode)) {
       fprintf(stderr, "Sorry, resolution %dx%d is not supported, yet\n", w, h);
       exit(EXIT_FAILURE);
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        exit(EXIT_FAILURE);
    }

    if (!(screen = SDL_SetVideoMode(w, h, 0, 0))) {
        exit(EXIT_FAILURE);
    }

    if (TTF_Init() == -1) {
        exit(EXIT_FAILURE);
    }

    vstretch = w / (float)800;
    hstretch = h / (float)600;
    printf("Stretches are %f x %f\n", vstretch, hstretch);

    atexit(SDL_Quit);

    /* load macro */
#define LOAD(a, b) { snprintf(buf, BUFLEN, "%s%d_%d/%s", DATADIR, w, h, a); \
    if ((temp = IMG_Load(buf))) \
    { b = SDL_DisplayFormat(temp); SDL_FreeSurface(temp); } \
    else { fprintf(stderr, "Could not load file >>%s<<: %s\n", a, IMG_GetError()); exit(EXIT_FAILURE); } }

    /* load all images */
    LOAD(ACT_LIFEBELT, act_lifebelt)
    LOAD(PAS_LIFEBELT, pas_lifebelt)
    LOAD(ACT_BORDER, act_border)
    LOAD(DEFAVA, defava)
    LOAD(DROWNAVA, drownava)
    LOAD(AVABOX, avabox)
    LOAD(HUD, hud)
    LOAD(PCARD, pcard)
    LOAD(WCARD, wcard)
    LOAD(TABLE, table)
#undef LOAD

    /* load font */
#define LOAD(a, b) { if(!(a = TTF_OpenFont(DEFFONT, b * hstretch))) \
    { fprintf(stderr, "Could not load file >>%s<<: %s\n", DEFFONT, TTF_GetError()); exit(EXIT_FAILURE); } }

    LOAD(font, FONTSIZE)
    LOAD(font2, FONTSIZE2)
    LOAD(font3, FONTSIZE3)
#undef LOAD

    ckey = SDL_MapRGB(screen->format, 0, 0, 0);
    SDL_SetColorKey(act_lifebelt, SDL_SRCCOLORKEY | SDL_RLEACCEL, ckey);
    SDL_SetColorKey(pas_lifebelt, SDL_SRCCOLORKEY | SDL_RLEACCEL, ckey);
    SDL_SetColorKey(avabox, SDL_SRCCOLORKEY | SDL_RLEACCEL, ckey);
    SDL_SetColorKey(hud, SDL_SRCCOLORKEY | SDL_RLEACCEL, ckey);
    SDL_SetColorKey(table, SDL_SRCCOLORKEY | SDL_RLEACCEL, ckey);
    SDL_SetColorKey(pcard, SDL_SRCCOLORKEY | SDL_RLEACCEL, ckey);
    SDL_SetColorKey(wcard, SDL_SRCCOLORKEY | SDL_RLEACCEL, ckey);

    return screen;
}

void draw_background(SDL_Surface *s, unsigned x, unsigned y)
{
    SDL_Rect r;

    r.x = x;
    r.y = y;
    SDL_BlitSurface(table, 0, s, &r);
}

void draw_hud(SDL_Surface *s, unsigned x, unsigned y)
{
    SDL_Rect r;

    r.x = x + hstretch * 18;
    r.y = y + vstretch * 385;
    SDL_BlitSurface(hud, 0, s, &r);
}

void create_playerbox(SDL_Surface *s, char *name, int x, int y, char *avatar, int lifebelts, bool dead)
{
    int i;
    SDL_Rect r;
    SDL_Surface *txt;

    /* draw border */
    r.x = x;
    r.y = y;

    SDL_BlitSurface(act_border, 0, s, &r);

    r.x += hstretch * 5;
    r.y += vstretch * 5;
    if (dead) {
        SDL_BlitSurface(drownava, 0, s, &r);
    } else if (!avatar) {
        SDL_BlitSurface(defava, 0, s, &r);
    } else {
        /* FIXME: load real ava */
    }
    SDL_BlitSurface(avabox, 0, s, &r);

    txt = TTF_RenderText_Blended(font, "Wasser", font_fg);
    r.x += hstretch * 77;
    r.y += vstretch * 37;
    SDL_BlitSurface(txt, 0, s, &r);
    SDL_FreeSurface(txt);

    txt = TTF_RenderText_Blended(font, "Punkte", font_fg);
    r.x += hstretch * 1;
    r.y += vstretch * 50;
    SDL_BlitSurface(txt, 0, s, &r);
    SDL_FreeSurface(txt);
    
    if (!name) name = "DUMMY";
    txt = TTF_RenderText_Blended(font, name, font_fg);
    r.x -= hstretch * 70;
    SDL_BlitSurface(txt, 0, s, &r);
    SDL_FreeSurface(txt);
    
    /* draw lifebelts */
    for (i = 0; i < lifebelts; i++)
        add_lifebelt(s, x, y, i);
}

int set_lifebelts(SDL_Surface *s, unsigned x, unsigned y, unsigned n, unsigned max)
{
    int i, j;
    SDL_Rect r;

    /* draw lifebelts */
    r.x = x + hstretch * 5;
    r.y = y + vstretch * 106;
    for (i = j = 0; i < max; j = ++i) {
#define BELTSIZE 24
        if (i == 5) {
	    r.x -= hstretch * 4 * BELTSIZE;
	    r.y += vstretch * BELTSIZE;
	} else if (i > 0) {
	    r.x += hstretch * BELTSIZE;
	}
	if (j >= n)
	    SDL_BlitSurface(pas_lifebelt, 0, s, &r);
	else
	    SDL_BlitSurface(act_lifebelt, 0, s, &r);
    }

    if (DEBUG)
        printf("Drawing %d/%d lifebelts done\n", n, max);

    return n;
}

#if 0
int kill_lifebelts(SDL_Surface *s, unsigned x, unsigned y)
{
    SDL_Rect r;

    r.x = x + 4 * hstretch;
    r.y = y + vstretch * 105;
    /*SDL_BlitSurface(box_lifebelt, 0, s, &r);*/

    return 1;
}
#endif

int add_lifebelt(SDL_Surface *s, unsigned x, unsigned y, unsigned n)
{
    SDL_Rect r;

    r.x = x + hstretch * 5;
    r.y = y + vstretch * 105;

    r.x += hstretch * (n % 5) * BELTSIZE;

    if (n >= 5)
        r.y += vstretch * BELTSIZE+4;
    SDL_BlitSurface(act_lifebelt, 0, s, &r);

    return 1;
}

int rm_lifebelt(SDL_Surface *s, unsigned x, unsigned y, unsigned n)
{
    SDL_Rect r;

    r.x = x + hstretch * 5;
    r.y = y + vstretch * 105;

    r.x += hstretch * (n % 5) * BELTSIZE;

    if (n >= 5)
        r.y += vstretch * BELTSIZE+4;
#undef BELTSIZE

    SDL_BlitSurface(pas_lifebelt, 0, s, &r);

    return 1;
}

int set_wlevel(SDL_Surface *s, unsigned x, unsigned y, unsigned n)
{
    SDL_Rect r;
    SDL_Surface *txt;
    char buf[5];

    snprintf(buf, 5, "%i", n);
    txt = TTF_RenderText_Blended(font2, buf, font_fg);

    r.x = x + hstretch * 94;
    r.y = y + vstretch * 17;
    SDL_BlitSurface(txt, 0, s, &r);
    SDL_FreeSurface(txt);

    return n;
}

int set_points(SDL_Surface *s, unsigned x, unsigned y, int n)
{
    SDL_Rect r;
    SDL_Surface *txt;
    char buf[5];

    snprintf(buf, 5, "%i", n);
    txt = TTF_RenderText_Blended(font2, buf, font_fg);

    r.x = x + hstretch * 94;
    r.y = y + vstretch * 67;
    SDL_BlitSurface(txt, 0, s, &r);
    SDL_FreeSurface(txt);

    return n;
}

int add_pcard(SDL_Surface *s, unsigned x, unsigned y, unsigned n, unsigned val)
{
    SDL_Rect r;
    SDL_Surface *txt;
    char buf[3];

    r.x = x + hstretch * 220;
    r.y = y + vstretch * 430;

    r.x += hstretch * n * 25;

    SDL_BlitSurface(pcard, 0, s, &r);

    snprintf(buf, 5, "%2i", val);
    txt = TTF_RenderText_Blended(font2, buf, font_fg);

    r.x += hstretch * 7;
    r.y += vstretch * 7;
    SDL_BlitSurface(txt, 0, s, &r);

    r.y += vstretch * 95;
    SDL_BlitSurface(txt, 0, s, &r);

    r.x += hstretch * 70;
    SDL_BlitSurface(txt, 0, s, &r);

    r.y -= vstretch * 95;
    SDL_BlitSurface(txt, 0, s, &r);
    SDL_FreeSurface(txt);

    return val;
}

int add_wcard(SDL_Surface *s, unsigned x, unsigned y, unsigned n, unsigned val)
{
    SDL_Rect r;
    SDL_Surface *txt;
    char buf[3];

    r.x = x + hstretch * 320;
    r.y = y + vstretch * 230;

    r.x += hstretch * n * 100;

    SDL_BlitSurface(wcard, 0, s, &r);

    snprintf(buf, 5, "%2i", val);
    txt = TTF_RenderText_Blended(font3, buf, font_fg);

    r.x += hstretch * 40;
    r.y += vstretch * 40;
    SDL_BlitSurface(txt, 0, s, &r);
    SDL_FreeSurface(txt);

    return val;
}

int add_pcard_played(SDL_Surface *s, unsigned x, unsigned y, int n, int val)
{
    SDL_Rect r;
    SDL_Surface *txt;
    char buf[5];

    r.x = x;
    r.y = y;

    switch(n) {
    case -1: /* the player */
        r.x += hstretch * 540;
        r.y += vstretch * 320;
        break;
    case 0: /* the opponents */
        r.x += hstretch * 50;
        r.y += vstretch * 200;
        break;
    case 1:
        r.x += hstretch * 260;
        r.y += vstretch * 100;
        break;
    case 2:
        r.x += hstretch * 490;
        r.y += vstretch * 100;
        break;
    case 3:
        r.x += hstretch * 650;
        r.y += vstretch * 200;
        break;
    }

    SDL_BlitSurface(pcard, 0, s, &r);

    snprintf(buf, 5, "%2i", val);
    txt = TTF_RenderText_Blended(font2, buf, font_fg);

    r.x += hstretch * 7;
    r.y += vstretch * 7;
    SDL_BlitSurface(txt, 0, s, &r);

    r.y += vstretch * 95;
    SDL_BlitSurface(txt, 0, s, &r);

    r.x += hstretch * 70;
    SDL_BlitSurface(txt, 0, s, &r);

    r.y -= vstretch * 95;
    SDL_BlitSurface(txt, 0, s, &r);
    SDL_FreeSurface(txt);

    return val;
}

TTF_Font *getfont(void)
{
  return font;
}

/* vim: set sw=4 ts=4 et fdm=syntax: */
