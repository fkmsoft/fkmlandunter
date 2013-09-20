#include "gui_util.h"

#define FONTSIZE (11)
#define FONTSIZE2 (18)
#define FONTSIZE3 (36)

#define BUFLEN 1024
#define GUI_UTIL_DEBUG 0

static void cleanup(void);

static void draw_background(SDL_Renderer *s, int x, int y);
static void draw_hud(SDL_Renderer *s, int x, int y);
static void create_playerbox(SDL_Renderer *s, char *name, int x, int y, char *avatar, int lifebelts, bool dead);

static int set_lifebelts(SDL_Renderer *s, int x, int y, int n, int max);
static int add_lifebelt(SDL_Renderer *s, int x, int y, int n);

static int set_wlevel(SDL_Renderer *s, int x, int y, int n);
static int set_points(SDL_Renderer *s, int x, int y, int n);

static int add_pcard(SDL_Renderer *s, int x, int y, int n, int val);
static int add_wcard(SDL_Renderer *s, int x, int y, int n, int val);
static int add_pcard_played(SDL_Renderer *s, int x, int y, int n, int val);

static SDL_Texture
    *act_lifebelt, *pas_lifebelt, *act_border, *drownava,
    *defava, *avabox, *hud, *pcard, *wcard, *table, *pcard_p;

static TTF_Font *font, *font2, *font3;
static SDL_Color font_fg = {0, 0, 0, 255};
/*static SDL_Color font_fg2 = {255, 255, 255, 255};*/

double hstretch, vstretch;

const int pbox_x = 624, pbox_y = 394;

SDL_Renderer *init_sdl(int w, int h, char *datadir, char *fontfile)
{
    char buf[BUFLEN], *p;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Surface *temp;
    struct stat s_stat;

    if (!datadir)
        datadir = DATADIR;

    if (stat(datadir, &s_stat) == -1 || !S_ISDIR(s_stat.st_mode)) {
        fprintf(stderr, "Datadir invalid: `%s'\n", datadir);
        exit(EXIT_FAILURE);
    }

    if ( !fontfile ) {
        fontfile = DEFFONT;
    } else {
        p = malloc(strlen(datadir) + strlen(fontfile) + 2);
        sprintf(p, "%s/%s", datadir, fontfile);
        fontfile = p;
        if (stat(fontfile, &s_stat) == -1) {
            p = malloc(strlen(fontfile) + 5);
            sprintf(p, "%s.ttf", fontfile);
            fprintf(stderr, "Font file `%s' does not exist, trying `%s'\n",
                    fontfile, p);
            free(fontfile);
            fontfile = p;
            if (stat(fontfile, &s_stat) == -1) {
                fprintf(stderr, "Font file `%s' does not exist, either, giving up.\n", fontfile);
                exit(EXIT_FAILURE);
            }
        }
    }

    snprintf(buf, BUFLEN, "%s/%d_%d", datadir, w, h);
    if (stat(buf, &s_stat) == -1 || !S_ISDIR(s_stat.st_mode)) {
       fprintf(stderr, "Sorry, resolution %dx%d is not supported, yet\n(searched for %s)\n", w, h, buf);
       exit(EXIT_FAILURE);
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Could not init SDL: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    snprintf(buf, BUFLEN, "%s/%s", datadir, ICON);
    temp = IMG_Load(buf);
    if (!temp) {
        fprintf(stderr, "Failed to load icon file %s.\n", buf);
        exit(1);
    }
    SDL_SetColorKey(temp, SDL_TRUE, SDL_MapRGB(temp->format, 0, 255, 0));
    /*
    SDL_FreeSurface(temp);
    */

    if (!(window = SDL_CreateWindow("Fkmlandunter", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                        w, h, 0))) {
        fprintf(stderr, "Could not init video: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    SDL_SetWindowIcon(window, temp);
    renderer = SDL_CreateRenderer(window, -1, 0);

    if (TTF_Init() == -1) {
        fprintf(stderr, "Could not init SDL_tff\n");
        exit(EXIT_FAILURE);
    }

    /*SDL_WM_SetCaption("Fkmlandunter", "Fkmlandunter");*/

    vstretch = w / (float)800;
    hstretch = h / (float)600;
    if (GUI_UTIL_DEBUG)
        printf("Stretches are %f x %f\n", vstretch, hstretch);

    atexit(SDL_Quit);

    /* load macro */
#define LOAD(a, b) { snprintf(buf, BUFLEN, "%s/%d_%d/%s", datadir, w, h, a); \
    if ((temp = IMG_Load(buf))) { \
        /*SDL_SetSurfaceAlphaMod(temp, 0);*/ \
        /*temp2 = SDL_DisplayFormatAlpha(temp);*/ \
        b = SDL_CreateTextureFromSurface(renderer, temp); \
        SDL_FreeSurface(temp); \
    } else { \
        fprintf(stderr, "Could not load file `%s': %s\n", a, IMG_GetError()); exit(EXIT_FAILURE); \
    } }

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
#define LOAD(a, b) { if(!(a = TTF_OpenFont(fontfile, b * hstretch))) \
    { fprintf(stderr, "Could not load file `%s': %s\n", fontfile, TTF_GetError()); exit(EXIT_FAILURE); } }

    LOAD(font, FONTSIZE)
    LOAD(font2, FONTSIZE2)
    LOAD(font3, FONTSIZE3)
#undef LOAD

    atexit(cleanup);

    return renderer;
}

static void draw_background(SDL_Renderer *s, int x, int y)
{
    SDL_Rect r;

    r.x = x;
    r.y = y;
    SDL_RenderCopy(s, table, 0, &r);
}

static void draw_hud(SDL_Renderer *s, int x, int y)
{
    SDL_Rect r;
    /*
    SDL_Surface *txt;
    SDL_Texture *text;
    */

    r.x = x + hstretch * 18;
    r.y = y + vstretch * 385;
    SDL_RenderCopy(s, hud, 0, &r);

    /*
    txt = TTF_RenderText_Blended(font, "- chat disabled -", font_fg);
    text = SDL_CreateTextureFromSurface(s, txt);
    r.x = x + 60 * hstretch;
    r.y = y + 410 * vstretch;
    text = SDL_CreateTextureFromSurface(s, txt);
    SDL_RenderCopy(s, txt, 0, &r);
    SDL_FreeSurface(txt);
    */
}

static void create_playerbox(SDL_Renderer *s, char *name, int x, int y, char *avatar, int lifebelts, bool dead)
{
    int i;
    SDL_Rect r;
    SDL_Surface *txt;
    SDL_Texture *text;

    /* draw border */
    r.x = x;
    r.y = y;

    SDL_RenderCopy(s, act_border, 0, &r);

    r.x += hstretch * 5;
    r.y += vstretch * 5;
    if (dead) {
        SDL_RenderCopy(s, drownava, 0, &r);
    } else if (!avatar) {
        SDL_RenderCopy(s, defava, 0, &r);
    } else {
        /* FIXME: load real ava */
    }
    SDL_RenderCopy(s, avabox, 0, &r);

    txt = TTF_RenderText_Blended(font, "Wasser", font_fg);
    text = SDL_CreateTextureFromSurface(s, txt);
    r.x += hstretch * 76;
    r.y += vstretch * 35;
    SDL_RenderCopy(s, text, 0, &r);
    SDL_FreeSurface(txt);

    txt = TTF_RenderText_Blended(font, "Punkte", font_fg);
    text = SDL_CreateTextureFromSurface(s, txt);
    /* r.x += hstretch * 1; */
    r.y += vstretch * 50;
    SDL_RenderCopy(s, text, 0, &r);
    SDL_FreeSurface(txt);
    
    txt = TTF_RenderText_Blended(font, name, font_fg);
    text = SDL_CreateTextureFromSurface(s, txt);
    r.x -= hstretch * 70;
    SDL_RenderCopy(s, text, 0, &r);
    SDL_FreeSurface(txt);
    
    /* draw lifebelts */
    for (i = 0; i < lifebelts; i++)
        add_lifebelt(s, x, y, i);
}

static int set_lifebelts(SDL_Renderer *s, int x, int y, int n, int max)
{
    int i, j;
    SDL_Rect r;

    if (max > 12 || n > 12 || max < 0 || n < -1) {
        fprintf(stderr, "Error: illegal number of lifebelts %d/%d\n", n, max);
        return -1;
    }

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
	    SDL_RenderCopy(s, pas_lifebelt, 0, &r);
	else
	    SDL_RenderCopy(s, act_lifebelt, 0, &r);
    }

    if (GUI_UTIL_DEBUG)
        printf("Drawing %d/%d lifebelts done\n", n, max);

    return n;
}

static int add_lifebelt(SDL_Renderer *s, int x, int y, int n)
{
    SDL_Rect r;

    r.x = x + hstretch * 5;
    r.y = y + vstretch * 105;

    r.x += hstretch * (n % 5) * BELTSIZE;

    if (n >= 5)
        r.y += vstretch * BELTSIZE+4;
    SDL_RenderCopy(s, act_lifebelt, 0, &r);
#undef BELTSIZE

    return 1;
}

static int set_wlevel(SDL_Renderer *s, int x, int y, int n)
{
    SDL_Rect r;
    SDL_Surface *txt;
    SDL_Texture *text;
    char buf[5];

    snprintf(buf, 5, "%i", n);
    txt = TTF_RenderText_Blended(font2, buf, font_fg);

    r.x = x + hstretch * 94;
    r.y = y + vstretch * 17;
    text = SDL_CreateTextureFromSurface(s, txt);
    SDL_RenderCopy(s, text, 0, &r);
    SDL_FreeSurface(txt);

    return n;
}

static int set_points(SDL_Renderer *s, int x, int y, int n)
{
    SDL_Rect r;
    SDL_Surface *txt;
    SDL_Texture *text;
    char buf[5];

    snprintf(buf, 5, "%i", n);
    txt = TTF_RenderText_Blended(font2, buf, font_fg);

    r.x = x + hstretch * 94;
    r.y = y + vstretch * 67;
    text = SDL_CreateTextureFromSurface(s, txt);
    SDL_RenderCopy(s, text, 0, &r);
    SDL_FreeSurface(txt);

    return n;
}

static int add_pcard(SDL_Renderer *s, int x, int y, int n, int val)
{
    SDL_Rect r;
    SDL_Surface *txt;
    SDL_Texture *text;
    char buf[3];

    r.x = x + hstretch * 220;
    r.y = y + vstretch * 430;

    r.x += hstretch * n * 25;

    SDL_RenderCopy(s, pcard, 0, &r);

    snprintf(buf, 5, "%2i", val);
    txt = TTF_RenderText_Blended(font2, buf, font_fg);
    text = SDL_CreateTextureFromSurface(s, txt);

    r.x += hstretch * 7;
    r.y += vstretch * 7;
    SDL_RenderCopy(s, text, 0, &r);

    r.y += vstretch * 95;
    SDL_RenderCopy(s, text, 0, &r);

    r.x += hstretch * 70;
    SDL_RenderCopy(s, text, 0, &r);

    r.y -= vstretch * 95;
    SDL_RenderCopy(s, text, 0, &r);
    SDL_FreeSurface(txt);

    return val;
}

static int add_wcard(SDL_Renderer *s, int x, int y, int n, int val)
{
    SDL_Rect r;
    SDL_Surface *txt;
    SDL_Texture *text;
    char buf[3];

    r.x = x + hstretch * 305;
    r.y = y + vstretch * 230;

    r.x += hstretch * n * 100;

    SDL_RenderCopy(s, wcard, 0, &r);

    snprintf(buf, 5, "%2i", val);
    txt = TTF_RenderText_Blended(font3, buf, font_fg);
    text = SDL_CreateTextureFromSurface(s, txt);

    r.x += hstretch * 40;
    r.y += vstretch * 40;
    SDL_RenderCopy(s, text, 0, &r);
    SDL_FreeSurface(txt);

    return val;
}

static int add_pcard_played(SDL_Renderer *s, int x, int y, int n, int val)
{
    SDL_Rect r;
    SDL_Surface *txt;
    SDL_Texture *text;
    char buf[5];

    if (val == 0)
        return -1;

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

    SDL_RenderCopy(s, pcard_p, 0, &r);

    snprintf(buf, 5, "%2i", val);
    txt = TTF_RenderText_Blended(font, buf, font_fg);
    text = SDL_CreateTextureFromSurface(s, txt);

    r.x += hstretch * 2;
    r.y += vstretch * 2;
    SDL_RenderCopy(s, text, 0, &r);

    r.y += vstretch * 48;
    SDL_RenderCopy(s, text, 0, &r);

    r.x += hstretch * 35;
    SDL_RenderCopy(s, text, 0, &r);

    r.y -= vstretch * 48;
    SDL_RenderCopy(s, text, 0, &r);
    SDL_FreeSurface(txt);

    /* big number in the center */
    txt = TTF_RenderText_Blended(font2, buf, font_fg);
    text = SDL_CreateTextureFromSurface(s, txt);
    r.y += vstretch * 20;
    r.x -= hstretch * 18;
    SDL_RenderCopy(s, text, 0, &r);
    SDL_FreeSurface(txt);

    return val;
}

TTF_Font *getfont(void)
{
  return font;
}

void pre_render(SDL_Renderer *s, char *name)
{
    draw_background(s, 0, 0);
    draw_hud(s, 0, 0);
    create_playerbox(s, name, hstretch * pbox_x, vstretch * pbox_y, 0, 0, false);
    set_wlevel(s, 0, 0, 0);
    set_points(s, 0, 0, 0);
}

void render(SDL_Renderer *s, gamestr *g, int pos, int *startbelts)
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

int card_select(int x, int y, int *deck)
{
    int i, c;

    c = -1;
    if (x > hstretch * 222 && x < hstretch * 590 &&
        y > vstretch * 430 && y < vstretch * 545) {
        c = (x - hstretch * 220) / (hstretch * 25);

        if (c > 11) {
            i = 3 + 11 - c;
            c = 11;
        } else {
            i = 3;
        }

        /* this is too close */
        if ((int)(x - hstretch * 220) % (int)(hstretch * 25) > hstretch * 22)
            i--;

        for (; i && c && !deck[c]; i--)
            c--;

        if (!deck[c])
            c = -1;
    }

    return c;
}

void game_over(SDL_Renderer *s, gamestr *g, int pos, int x, int y)
{
    int i, maxp, maxi;
    char buf[BUFLEN];
    SDL_Surface *txt;
    SDL_Texture *text;
    SDL_Rect r;

    maxp = g->villain[0].points;
    maxi = 0;
    for (i = 1; i < g->count; i++)
        if (g->villain[i].points > maxp) {
            maxp = g->villain[i].points;
            maxi = i;
        }

    if (maxi == pos)
        sprintf(buf, "You win the game with %d points!\n", maxp);
    else
        sprintf(buf, "%s wins the game with %d points!\n", g->villain[maxi].name, maxp);

    txt = TTF_RenderText_Blended(font3, buf, font_fg);
    text = SDL_CreateTextureFromSurface(s, txt);
    r.x = x + 100 * hstretch;
    r.y = y + 300 * vstretch;
    SDL_RenderCopy(s, text, 0, &r);
    SDL_FreeSurface(txt);
}

static void cleanup(void)
{
    /* free all the stuff we allocated before */
    SDL_DestroyTexture(act_lifebelt);
    SDL_DestroyTexture(pas_lifebelt);
    SDL_DestroyTexture(act_border);
    SDL_DestroyTexture(drownava);
    SDL_DestroyTexture(defava);
    SDL_DestroyTexture(avabox);
    SDL_DestroyTexture(hud);
    SDL_DestroyTexture(pcard);
    SDL_DestroyTexture(wcard);
    SDL_DestroyTexture(table);
    SDL_DestroyTexture(pcard_p);

    TTF_CloseFont(font);
    TTF_CloseFont(font2);
    TTF_CloseFont(font3);
}

/* vim: set sw=4 ts=4 et fdm=syntax: */
