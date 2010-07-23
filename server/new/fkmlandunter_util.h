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

int *shuffle(int num);
deck *create_decks(int num);
deck *deck_rotate(deck *decks, int d, int num);
player *create_player(int fd);

#endif /* FKMLANDUNTER_UTIL_H */

/* vim: set sw=4 ts=4 fdm=syntax: */
