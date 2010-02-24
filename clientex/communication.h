/* communication.h
 *
 * communication interface
 *
 * (c) Fkmsoft, 2010
 */

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <sys/time.h>

#define BUF_SIZE    (1024)
#define max(A,B)    ((A) > (B) ? (A) : (B))

typedef struct {
    int points;
    int water_level;
    bool dead;
    char *name;
    int weathercards[12];
    int lifebelts;
} player;

typedef struct {
    player *p;
    int count;
} opponents;

/* GAMEPLAY */

/* Create and initialize one player */
player *create_player();

/* create opponents from string s */
opponents *parse_start(char *s);

/* parse deck from string s to player p */
void parse_deck(player *p, char *s);

/* parse rings from string s to opponents o */
void parse_rings(opponents *o, char *s);

/* parse weathercards from string s to w_card */
void parse_weather(int *w_card, char *s);

/* parse waterlevel from string s to opponents o */
void parse_wlevels(opponents *o, char *s);

/* parse points from string s to opponents o */
void parse_points(opponents *o, char *s);

/* NETWORK */

/* create and return socket */
int create_sock();

/* connect to socket sock - return -1 if attempt fails, -2 if host unknown */
int connect_socket(int sock, char *serv_addr, unsigned short port);

/* receive BUF_SIZE chars from *fp */
char *receive_from(FILE *fp);

/* send formated string *fmt to *fp */
void send_to(FILE *fp, char *fmt, ...);

/* select from fd inputA and inputB */
int select_input(int inputA, int inputB);

#endif
/* vim: set sw=4 ts=4 et fdm=syntax: */
