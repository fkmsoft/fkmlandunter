/* fkml_server.h
 *
 * (c) Fkmsft, 2010
 */

#ifndef FKML_SERVER
#define FKML_SERVER

#include <stdarg.h>
#include <stdbool.h>

#include "ncom.h"

#define MAXLEN (128)

enum SERVER_COMMANDS { ACK, START, WEATHER, RINGS, DECK, FAIL, WLEVELS, POINTS,
    TERMINATE, MSGFROM, JOIN, LEAVE, PLAYED };

enum CLIENT_COMMAND { LOGIN, START_C, PLAY, MSG, LOGOUT, INVALID };

/* Return number of first client that has send input */
int poll_for_input(fkml_server *s);

/* Remove any chars in evil from the end of str */
void trim(char *str, char *evil);

/* Create a fkmlandunter for players players on port port */
fkml_server *init_server(unsigned int port, unsigned int players);

/* Wait for a new player to connect and add it to server s */
bool fkml_addplayer(fkml_server *s);

/* Return client command of s */
enum CLIENT_COMMAND get_client_cmd(char *s);

void send_cmd(fkml_server *s, int c, enum SERVER_COMMANDS cmd, char *msg, ...);

/* Add a new client described by file descriptor fd to server s */
int fkml_addclient(fkml_server *s, int fd);

/* Remove client c from server s */
void fkml_rmclient(fkml_server *s, int c);

/* Receive message from client c on server s */
char *fkml_recv(fkml_server *s, int c);

/* Send message msg to client c on server s */ /* XXX outplemented */
int fkml_puts(fkml_server *s, int c, char *msg);

/* Send formatted message to client c on server s */
void fkml_printf(fkml_server *s, int c, char *msg, ...);

/* process message msg from client c on server s */
int fkml_process(fkml_server *s, int c, char *msg);

/* Handle command cmd with arguments args in server s */
int fkml_handle_command(fkml_server *s, char *cmd, char *args);

/* Print debug info about client c on server s */
void fkml_printclients(fkml_server *);

/* Tell a client that connection is impossible and disconnect */
void fkml_failclient(fkml_server *);

/* Shut down server s */
void fkml_shutdown(fkml_server *s, char *msg);

#endif /* FKML_SERVER */

/* vim: set sw=4 ts=4 fdm=syntax: */
