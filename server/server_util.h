/* server_util.h
 *
 * (c) Fkmsoft, 2010
 */

#ifndef SERVER_UTIL_H
#define SERVER_UTIL_H

/*#include "communication.h"*/
#include "ncom.h"

void create_players(player *p_array/*, deck *d_array*/, int num);
deck *create_decks(int num);
int *shuffle(int num);
deck *deck_rotate(deck *decks, int d, int num);
void free_decks(fkml_server *s);
void hand_decks(player *players, deck *decks, int num);

#endif
/* vim: set sw=4 ts=4 fdm=syntax: */
