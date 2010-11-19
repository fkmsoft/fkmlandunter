/* fkmserver.h
 *
 * A Fkmbab server.
 *
 * (c) 2010, Fkmsoft
 */

#ifndef _FKMSERVER_H_
#define _FKMSERVER_H_

/* for POLLRDHUP */
#define _GNU_SOURCE

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <poll.h>
#include <netinet/in.h>
#include <errno.h>

#include "fkmlist.h"

#define MAXMSGLEN (1024)

/* the data structure of any fkmserver */
typedef struct {

    /* socket holds a file descriptor to the socket the fkmserver is listening
     * on */
    int socket;

    /* connections holds the number of clients connected to the fkmserver */
    int connections;

    /* clients holds the list of clients connected to the fkmserver.
     * A client can be any kind of data structure (struct) that has a field
     * named 'fd' (this is where the file descriptor of the client socket will
     * be stored */
    fkmlist *clients;
    int (*getfd)(void *);
} fkmserver;

/* create a new fkmserver listening on port port and return a pointer
 * to its data structure */
fkmserver *fkmserver_init(int port, int (*getfd)(void *));

/* stop the fkmserver pointed to by s and free all its resources */
void fkmserver_stop(fkmserver *s);

/* add a client to the list of clients.
 * create_client is a pointer to a function that will fill the client data
 * structure. It can do whatever necessary, it only must fill the fd field
 * with the value specified in fd. fd must be a file descriptor that is
 * capable of reading and writing. */
bool fkmserver_addnetc(fkmserver *s, void *(create_client)(int fd));

void fkmserver_refusenetc(fkmserver *s, char *message);

bool fkmserver_addpipec(fkmserver *s, char *path);

/* get a pointer to the client at position index in the clientlist */
void *fkmserver_getc(fkmserver *s, int index);

/* remove the client identified by fd from the list of clients. */
bool fkmserver_rmc(fkmserver *s, int fd);
bool fkmserver_rmidxc(fkmserver *s, int index);

/* send the message pointed to by msg to the client identified by fd. */
bool fkmserver_cfdsend(fkmserver *s, int fd, char *msg);
bool fkmserver_cidxsend(fkmserver *s, int index, char *msg);

/* receive one line of input from the client identified by fd. */
char *fkmserver_cfdrecv(fkmserver *s, int fd);
char *fkmserver_cidxrecv(fkmserver *s, int index);

/* poll for input and return the index of a client that has input ready to be
 * read, a negative index of a client that disconnected or 0 when a new client
 * attempts to connect to the server. */
int fkmserver_poll(fkmserver *s);

/* save file descriptors of the server socket and all
 * connected clients in buf */
void fkmserver_exportfds(fkmserver *s, int *buf);

#endif /* _FKMSERVER_H_ */

/* vim: set sw=4 ts=4 et fdm=syntax: */
