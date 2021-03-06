/* communication.h
 *
 * communication interface
 *
 * (c) Fkmsoft, 2010
 */

#ifndef PARSE_H
#define PARSE_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define TABSIZE     (8)
#define MAXNICK     (30)
#define BUF_SIZE    (1024)
#define max(A,B)    ((A) > (B) ? (A) : (B))


typedef struct {
    int   points;
    int   water_level;
    bool  dead;
    char *name;
    int   weathercards[12];
    int   lifebelts;
    int   tabnum;
    int   played;
} player;

typedef struct {
    player  player;
    player *villain;
    int     count;
    int     w_card[2];
    int     round;
    bool    deck,
            rings,
            weather,
            wlevel,
            points;
    bool    message;
    char   *msg_from;
    char   *msg_data;
    int     tabnum;
} gamestr;

/* GAMEPLAY */

/* Create and initialize game */
gamestr *create_game();

/* initialise gamestr->villains from string s
 * returns position of player (0..4) */
int parse_start(gamestr *game, char *s);

/* parse server command */
int parse_cmd(gamestr *g, char *s);

#endif /* PARSE_H */
/* vim: set sw=4 ts=4 et fdm=syntax: */
