/* fkml_client.h
 *
 * fkmlandunter client
 *
 * (c) Fkmsoft, 2010
 */

#ifndef FKML_CLIENT_H
#define FKML_CLIENT_H

/* fdopen */
#define _POSIX_SOURCE

#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include "interface.h"
#include "communication.h"

#define BUF_SIZE    (1024)
#define MAXNICK     (30)

void print_deck(gamestr *game);
void print_rings(gamestr *game);
void print_weathercards(gamestr *game);
void print_wlevel(gamestr *game);
void print_points(gamestr *game);

#endif
/* vim: set sw=4 ts=4 et fdm=syntax: */
