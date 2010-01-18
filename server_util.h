/* server_util.h
 *
 * (c) Fkmsoft, 2010
 */

#ifndef SERVER_UTIL_H
#define SERVER_UTIL_H

#include "communication.h"

player *create_players(int num);
deck *create_decks(int num);
int *shuffle(int num);

#endif
