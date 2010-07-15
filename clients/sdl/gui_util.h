#define _ISOC99_SOURCE
#include <stdio.h>
#include <sys/stat.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#define DATADIR "../../data/fkmlu/"

/* border */
#define ACT_BORDER ("rahmen_aktiv.png")
#define PAS_BORDER ("rahmen_fertig.png")

/* background */
#define TABLE ("tisch.png")
#define HUD ("hud.png")

/* name */
#define NAME_BORDER ("nutzername.png")

/* lifebelt */
#define BOX_LIFEBELT ("kasten.png")
#define PAS_LIFEBELT ("rettungsring_sw.png")
#define ACT_LIFEBELT ("rettungsring.png")

/* points */
#define POINTSBOX ("punktestand.png")

/* water level */
#define WLEVELBOX ("wasserlevel.png")

/* standard avatar */
#define DEFAVA ("nutzerbild.png")
#define AVABOX ("nutzerbild_rahmen.png")

/* cards */
#define PCARD ("punktekarte.png")

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
