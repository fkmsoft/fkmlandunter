/* ncom.h
 *
 * new communication
 *
 * (c) Fkmsft, 2010
 */

#ifndef _NCOM_H_
#define _NCOM_H_

#include <stdbool.h>
#include <stdio.h>

#define MAX_PLAYERS (5)

typedef struct { /* deck */
    int weathercards[12];
    int lifebelts;
} deck;

typedef struct { /* player */
    int points;
    deck current_deck;
    int water_level;
    bool dead;
    char *name;
    int fd;
    FILE *fp;
} player;

typedef struct { /* fkml_server */
    int socket;
    int connected;
    int water[24];
    player players[MAX_PLAYERS];
} fkml_server;

void show_startmsg(fkml_server *s, int p);
void print_deck(fkml_server *s, int p);
void show_weather(int min, int max, fkml_server *s, int p);
void show_waterlevels(fkml_server *s, int p);
void show_points(fkml_server *s, int p);
int read_weather(fkml_server *s, int p);

#endif /* _NCOM_H_ */

/* vim: set sw=4 ts=4 fdm=syntax: */
