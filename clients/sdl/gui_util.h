#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#define _ISOC99_SOURCE
#include <stdio.h>

#define DATADIR "../../data/fkmlu/"

/* border */
#define ACT_BORDER (DATADIR "rahmen_aktiv.png")
#define PAS_BORDER (DATADIR "rahmen_fertig.png")

/* background */
#define TABLE (DATADIR "tisch.png")
#define HUD (DATADIR "hud.png")

/* name */
#define NAME_BORDER (DATADIR "nutzername.png")

/* lifebelt */
#define BOX_LIFEBELT (DATADIR "kasten.png")
#define PAS_LIFEBELT (DATADIR "rettungsring_sw.png")
#define ACT_LIFEBELT (DATADIR "rettungsring.png")

/* points */
#define POINTSBOX (DATADIR "punktestand.png")

/* water level */
#define WLEVELBOX (DATADIR "wasserlevel.png")

/* standard avatar */
#define DEFAVA (DATADIR "nutzerbild.png")
#define AVABOX (DATADIR "nutzerbild_rahmen.png")

/* cards */
#define PCARD (DATADIR "punktekarte.png")

/* standard font */
#define DEFFONT (DATADIR "TOONISH.ttf")

SDL_Surface *init_sdl(int w, int h);

void create_playerbox(SDL_Surface *s, unsigned x, unsigned y, char *avatar, unsigned lifebelts);

int set_lifebelts(SDL_Surface *s, unsigned x, unsigned y, unsigned n, unsigned max);

int kill_lifebelts(SDL_Surface *s, unsigned x, unsigned y);
int add_lifebelt(SDL_Surface *s, unsigned x, unsigned y, unsigned n);
int rm_lifebelt(SDL_Surface *s, unsigned x, unsigned y, unsigned n);

int set_wlevel(SDL_Surface *s, unsigned x, unsigned y, unsigned n);
int set_points(SDL_Surface *s, unsigned x, unsigned y, int n);

int add_pcard(SDL_Surface *s, unsigned x, unsigned y, unsigned n);
TTF_Font *getfont(void);
