/* server_util.h
 *
 * (c) Fkmsoft, 2010
 */

#ifndef SERVER_UTIL_H
#define SERVER_UTIL_H

/*#include "communication.h"*/
#include "ncom.h"

void create_players(player *p_array, int num);
deck *create_decks(int num);
int *shuffle(int num);

#endif
/* vim: set sw=4 ts=4 fdm=syntax: */
