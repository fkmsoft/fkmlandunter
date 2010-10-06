/* fkmlandunter_util.h
 *
 * Fkmlandunter utility functions
 *
 * (c) Fkmsoft, 2010
 */

#ifndef FKMLANDUNTER_UTIL_H
#define FKMLANDUNTER_UTIL_H

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "fkmlandunter.h"

/* create integer array with numbers 1..num in random order */
int *shuffle(int num);

/* create num decks */
deck *create_decks(int num);

/* rotate the num decks decks by d positions */
deck *deck_rotate(deck *decks, int d, int num);

/* create an clean player with fd set to fd */
player *create_player(int fd);

#endif /* FKMLANDUNTER_UTIL_H */

/* vim: set sw=4 ts=4 fdm=syntax: */
