/* communication.h
 *
 * communication interface
 *
 * (c) Fkmsoft, 2010
 */

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <sys/time.h>
#include <stdio.h>

#include <unistd.h>
#include <poll.h>

#define MAXNICK     (30)
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
    player player;
    player *villain;
    int count;
    int w_card[2];
    int round;
    bool deck, rings, weather, wlevel, points;
    bool message;
    char *msg_from;
    char *msg_data;
} gamestr;

/* GAMEPLAY */

/* Create and initialize game */
gamestr *create_game();

/* initialise gamestr->villains from string s */
void parse_start(gamestr *game, char *s);

/* parse server command */
int parse_cmd(gamestr *g, char *s);

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

/* poll from fd inputA and inputB */
int poll_input(int inputA, int inputB);

#endif
/* vim: set sw=4 ts=4 et fdm=syntax: */
