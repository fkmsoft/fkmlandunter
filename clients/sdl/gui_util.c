#include "gui_util.h"

#define FONTSIZE (11)
#define FONTSIZE2 (15)
#define FONTSIZE3 (36)

#define BUFLEN 1024
#define DEBUG 0

static void cleanup(void);

static SDL_Surface  
    *act_lifebelt, *pas_lifebelt, *act_border, *drownava,
    *defava, *avabox, *hud, *pcard, *wcard, *table, *pcard_p;

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
    LOAD(PCARD_P, pcard_p)
    LOAD(TABLE, table)
#undef LOAD

    /* load font */
#define LOAD(a, b) { if(!(a = TTF_OpenFont(DEFFONT, b * hstretch))) \
    { fprintf(stderr, "Could not load file >>%s<<: %s\n", DEFFONT, TTF_GetError()); exit(EXIT_FAILURE); } }

    LOAD(font, FONTSIZE)
    LOAD(font2, FONTSIZE2)
    LOAD(font3, FONTSIZE3)
#undef LOAD

    atexit(cleanup);

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

void draw_background(SDL_Surface *s, int x, int y)
{
    SDL_Rect r;

    r.x = x;
    r.y = y;
    SDL_BlitSurface(table, 0, s, &r);
}

void draw_hud(SDL_Surface *s, int x, int y)
{
    SDL_Rect r;
    SDL_Surface *txt;

    r.x = x + hstretch * 18;
    r.y = y + vstretch * 385;
    SDL_BlitSurface(hud, 0, s, &r);

    txt = TTF_RenderText_Blended(font, "- chat disabled -", font_fg);
    r.x = x + 60 * hstretch;
    r.y = y + 410 * vstretch;
    SDL_BlitSurface(txt, 0, s, &r);
    SDL_FreeSurface(txt);
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
    r.x += hstretch * 76;
    r.y += vstretch * 35;
    SDL_BlitSurface(txt, 0, s, &r);
    SDL_FreeSurface(txt);

    txt = TTF_RenderText_Blended(font, "Punkte", font_fg);
    /* r.x += hstretch * 1; */
    r.y += vstretch * 50;
    SDL_BlitSurface(txt, 0, s, &r);
    SDL_FreeSurface(txt);
    
    txt = TTF_RenderText_Blended(font, name, font_fg);
    r.x -= hstretch * 70;
    SDL_BlitSurface(txt, 0, s, &r);
    SDL_FreeSurface(txt);
    
    /* draw lifebelts */
    for (i = 0; i < lifebelts; i++)
        add_lifebelt(s, x, y, i);
}

int set_lifebelts(SDL_Surface *s, int x, int y, int n, int max)
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
int kill_lifebelts(SDL_Surface *s, int x, int y)
{
    SDL_Rect r;

    r.x = x + 4 * hstretch;
    r.y = y + vstretch * 105;
    /*SDL_BlitSurface(box_lifebelt, 0, s, &r);*/

    return 1;
}
#endif

int add_lifebelt(SDL_Surface *s, int x, int y, int n)
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

int rm_lifebelt(SDL_Surface *s, int x, int y, int n)
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

int set_wlevel(SDL_Surface *s, int x, int y, int n)
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

int set_points(SDL_Surface *s, int x, int y, int n)
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

int add_pcard(SDL_Surface *s, int x, int y, int n, int val)
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

int add_wcard(SDL_Surface *s, int x, int y, int n, int val)
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

int add_pcard_played(SDL_Surface *s, int x, int y, int n, int val)
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
        r.x += hstretch * 100;
        r.y += vstretch * 200;
        break;
    case 1:
        r.x += hstretch * 260;
        r.y += vstretch * 150;
        break;
    case 2:
        r.x += hstretch * 490;
        r.y += vstretch * 150;
        break;
    case 3:
        r.x += hstretch * 650;
        r.y += vstretch * 200;
        break;
    }

    SDL_BlitSurface(pcard_p, 0, s, &r);

    snprintf(buf, 5, "%2i", val);
    txt = TTF_RenderText_Blended(font, buf, font_fg);

    r.x += hstretch * 2;
    r.y += vstretch * 2;
    SDL_BlitSurface(txt, 0, s, &r);

    r.y += vstretch * 48;
    SDL_BlitSurface(txt, 0, s, &r);

    r.x += hstretch * 35;
    SDL_BlitSurface(txt, 0, s, &r);

    r.y -= vstretch * 48;
    SDL_BlitSurface(txt, 0, s, &r);
    SDL_FreeSurface(txt);

    return val;
}

TTF_Font *getfont(void)
{
  return font;
}

void render(SDL_Surface *s, gamestr *g, int pos, int *startbelts)
{
    int x, y, i, j;
    player p;

    draw_background(s, 0, 0);

    p = g->villain[pos];
    add_pcard_played(s, 0, 0, -1, p.played);
    draw_hud(s, 0, 0);

    create_playerbox(s, p.name, hstretch * pbox_x, vstretch * pbox_y, 0, 0, p.dead);
    set_points(s, hstretch * pbox_x, vstretch * pbox_y, p.points);
    if (p.dead) {
        set_lifebelts(s, hstretch * pbox_x, vstretch *pbox_y, 0, startbelts[pos]);
    } else {
        set_lifebelts(s, hstretch * pbox_x, vstretch *pbox_y, p.lifebelts, startbelts[pos]);
        set_wlevel(s, hstretch * pbox_x, vstretch * pbox_y, p.water_level);
    }

    add_wcard(s, 0, 0, 0, g->w_card[0]);
    add_wcard(s, 0, 0, 1, g->w_card[1]);

    x = 50 * hstretch;
    y = 10 * vstretch;
    j = 0;
    for (i = 0; i < g->count; i++) {
        if (i != pos) {
            p = g->villain[i];
            create_playerbox(s, p.name, x, y, 0, 0, p.dead);
            set_points(s, x, y, p.points);

            if (p.dead) {
                set_lifebelts(s, x, y, 0, startbelts[i]);
            } else {
                set_lifebelts(s, x, y, p.lifebelts, startbelts[i]);
                set_wlevel(s, x, y, p.water_level);
            }

            x += 200 * hstretch;
            add_pcard_played(s, 0, 0, j++, p.played);
        }
    }

    for (i = 0; i < 12; i++)
        if (g->player.weathercards[i])
            add_pcard(s, 0, 0, i, g->player.weathercards[i]);
}

static void cleanup(void)
{
    /* free all the stuff we allocated before */
    SDL_FreeSurface(act_lifebelt);
    SDL_FreeSurface(pas_lifebelt);
    SDL_FreeSurface(act_border);
    SDL_FreeSurface(drownava);
    SDL_FreeSurface(defava);
    SDL_FreeSurface(avabox);
    SDL_FreeSurface(hud);
    SDL_FreeSurface(pcard);
    SDL_FreeSurface(wcard);
    SDL_FreeSurface(table);
    SDL_FreeSurface(pcard_p);

    TTF_CloseFont(font);
    TTF_CloseFont(font2);
    TTF_CloseFont(font3);
}

/* vim: set sw=4 ts=4 et fdm=syntax: */
