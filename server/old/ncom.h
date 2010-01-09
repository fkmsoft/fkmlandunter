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
    int played;
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

/* Send start message to player p on server s */
void show_startmsg(fkml_server *s, int p);

/* Send startmessage to player p on server s */
void show_deck(fkml_server *s, int p);

/* Send startmessage to player p on server s */
void show_rings(fkml_server *s, int p);

/* Send new weather cards to player p on server s */
void show_weather(int min, int max, fkml_server *s, int p);

/* Send new waterlevels to player p on server s */
void show_waterlevels(fkml_server *s, int p);

/* Send point summary to player p on server s */
void show_points(fkml_server *s, int p);

/* Read the next weather card (or a bunch of other stuff)
 * from player p on server s */
int read_weather(fkml_server *s, int p);

/* Send join message to player p on server s */
void show_join(fkml_server *s, int p, int new);

/* Send leave message to player p on server s */
void show_leave(fkml_server *s, int p, int leaver);

/* Send played card of all players to player p on server s */
void show_played(fkml_server *s, int p);

#endif /* _NCOM_H_ */

/* vim: set sw=4 ts=4 fdm=syntax: */
