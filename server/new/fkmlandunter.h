/* fkmlandunter.h
 *
 * The implementation of the Fkmlandunter game.
 *
 * (c) 2010, Fkmsoft
 */

#ifndef _FKMLANDUNTER_H
#define _FKMLANDUNTER_H

#include <stdbool.h>

#include "fkmserver.h"
#include "communication.h"

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
} player;

#include "fkmlandunter_util.h"

bool fkmlandunter_play(fkmserver *s, int playerlimit);

#endif /* _FKMLANDUNTER_H */

/* vim: set sw=4 ts=4 et fdm=syntax: */
