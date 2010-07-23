/* communication.h
 *
 * Communication functions for Fkmlandunter
 *
 * (c) 2010, Fkmsoft
 */

#ifndef _COMMUNICATION_H_
#define _COMMUNICATION_H_

/* for vsnprintf */
#define _GNU_SOURCE

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>

#include "fkmserver.h"
#include "fkmlandunter.h"

#define MSGDELIM ('\n')

/* Send start message to player p on server s */
void send_startmsgs(fkmserver *s);

/* Send startmessage to player p on server s */
void send_decks(fkmserver *s);

/* Send startmessage to player p on server s */
void send_lifebelts(fkmserver *s);

/* Send new weather cards to player p on server s */
void send_weather(fkmserver *s, int min, int max);

/* Send new waterlevels to player p on server s */
void send_wlevels(fkmserver *s);

/* Send point summary to player p on server s */
void send_points(fkmserver *s);

/* Send played card of all players to player p on server s */
void send_played(fkmserver *s);

/* Read the next weather card (or a bunch of other stuff)
 * from player p on server s */
int parse_game_input(fkmserver *s, int p);

/* parse pre game input from player with index p on server s.
 * return -1 if player left
 * return 1 if player logged in
 * return 3 on start request
 * return 0 otherwise
 */
int parse_pre_game_input(fkmserver *s, int p);

/* Send join message to player p on server s */
void send_join(fkmserver *s, int joiner);

/* Send leave message to player p on server s */
void send_leave(fkmserver *s, int leaver);

/* Send the last card everybody played */
void send_played(fkmserver *s);

void send_message(fkmserver *s, int author, char *msg);

void send_op_message(fkmserver *s, char *msg);

void rmplayer(fkmserver *s, int p);

void send_refuse(fkmserver *s);

/* disconnect all connected players, that have not logged in, or all */
void disconnect_fuckers(fkmserver *s, bool all);

#endif /* _COMMUNICATION_H_ */

/* vim: set sw=4 ts=4 fdm=syntax: */
