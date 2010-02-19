/* communication.h
 *
 * Die Kommunikationsschnittstelle von
 * Fkmlandunter
 *
 * (c) Fkmsoft, 2010
 */

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#include "interface.h"

#define	MAXNICK	(30)

typedef struct {
	int	points;
	int	water_level;
	bool	dead;
	char	*name;
	int	weathercards[12];
	int	lifebelts;
} player;

typedef struct {
	player	*opponent;
	int	count;
} opponents;


/* Create and initialize one player */
player *create_player();

/* request nickname from user in GAME_BOX */
char *request_nick();

/* parse deck from string s to player p */
void parse_deck(player *p, char *s);

opponents *parse_start(char *s);

void parse_rings(opponents *o, char *s);

void parse_weather(int *w_card, char *s);

void parse_wlevels(opponents *o, char *s);

void parse_points(opponents *o, char *s);

/*void show_names(int n, char **names, player *p);*/
	/* nichts oder namen ausgeben */
	/* int n und n nullterminierte strings nach fd */
/*void anzahl_der_player(int num); XXX CRAP!*/

/*void print_player(player *p);*/

/*void show_waterlevel(int a, int b, player *p);*/
	/* 2 wasserstaende nach stdout */
	/* 2 ints nach fd */
/* int get_weather(player *p);*/
	/* do { lies ein int i von stdin} while(i nicht in deck); */
	/* 1 int von fd (bis es passt) */
/*void show_waterlevels(int *levels, player *p);*/
	/* S Wasserstande (mit namen) nach stdout */
	/* S ints nach fd */
/*void show_points(int *points, player *p);*/
	/* S Punktzahlen nach stdout */
	/* S ints nach fd */
/*int getint();*/

#endif
/* vim: set sw=4 ts=4 fdm=syntax: */
