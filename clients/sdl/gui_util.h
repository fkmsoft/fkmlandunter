/* gui_util.h
 *
 * (c) 2010, Fkmsoft
 */

#ifndef FKML_GUI_UTIL_H
#define FKML_GUI_UTIL_H

/* for snprintf */
#define _ISOC99_SOURCE
#include <stdio.h>
#include <sys/stat.h>
#include <stdbool.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#include "../communication.h"

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
#define DROWNAVA ("nutzerbild_d.png")
#define AVABOX ("nutzerbild_rahmen.png")

/* cards */
#define PCARD ("punktekarte.png")
#define WCARD ("wasserkarte.png")
#define PCARD_P ("punktekarte_p.png")

/* standard font */
#define DEFFONT (DATADIR "TOONISH.ttf")

extern const int pbox_x, pbox_y;

SDL_Surface *init_sdl(int w, int h);

TTF_Font *getfont(void);

void pre_render(SDL_Surface *s, char *name);
void render(SDL_Surface *s, gamestr *g, int pos, int *startbelts);

int card_select(int x, int y, int *deck);
void game_over(SDL_Surface *s, gamestr *g, int pos, int x, int y);

#endif /* FKML_GUI_UTIL_H */

/* vim: set sw=4 ts=4 et fdm=syntax: */
