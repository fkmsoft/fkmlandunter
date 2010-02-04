/* fkmsft
 *
 * fkmlandunter prtcl:
 * -----------------------------
 * -> LOGIN name
 * <- ACK nam
 * <- START 3 nam1 nam2 nam3
 *
 * <- WEATHER 7 8
 * <- DECK 3 1 2 56
 * -> PLAY 56
 * <- ACK 56
 * <- FAIL 56
 * <- WLEVELS 7 8 0
 * <- POINTS 1 2 -1
 *
 * -> LOGOUT bye
 * <- TERMINATE fuck off
 *
 * -> MSG fu all los0rZ
 * <- MSGFROM name fu all los0rZ
 * -----------------------------
 */

/* for fdopen() from stdio */
#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
/* for socket(): */
#include <sys/types.h>
#include <sys/socket.h>
/* for memset(): */
#include <string.h>
/* for struct sockaddr_in: */
#include <netinet/in.h>
/* for close(): */
#include <unistd.h>

#include "fkml_server.h"
#include "queue.h"

#define MAXLEN (128)

enum SERVER_COMMANDS { ACK, START, WEATHER, DECK, FAIL, WLEVELS, POINTS,
    TERMINATE, MSGFROM };

enum CLIENT_COMMAND { LOGIN, PLAY, MSG, LOGOUT, INVALID };

static char *server_command[] = { "ACK", "START", "WEATHER", "DECK", "FAIL",
    "WLEVELS", "POINTS", "TERMINATE", "MSGFROM" };

static char *client_command[] = { "LOGIN", "PLAY", "MSG", "LOGOUT" };

fkml_server *init_server(unsigned int port, unsigned int players)
{
    if (players > MAX_PLAYERS || players < 0)
        players = MAX_PLAYERS;

    fkml_server *server = malloc(sizeof(fkml_server));

    if (port < 1024)
        printf("Please choose a port > 1024 instead of %d\n",
                port);

    server->socket = socket(AF_INET, SOCK_STREAM, 0);
    server->connected = 0;

    struct sockaddr_in s_addr;
    memset(&s_addr, 0, sizeof(s_addr));

    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = INADDR_ANY;
    s_addr.sin_port = htons(port);

    bind(server->socket, (struct sockaddr *) &s_addr, sizeof(s_addr));
    listen(server->socket, players);

    puts("Server initialized, returning control");

    return server;
}

void fkml_addclient(fkml_server *s, int fd)
{
    s->clientfds[s->connected] = fd;
    s->clients[s->connected] = fdopen(fd, "a+");
    s->connected++;
}

void fkml_rmclient(fkml_server *s, int i)
{
    if (i < 0)
        return;

    /* printf("Removing client %i\n", i); */

    int n;
    for (n = i+1; n < MAX_PLAYERS && s->clients[n-1]; n++) {
        /* printf("Closing fd %d\n", s->clientfds[n-1]); */
        fclose(s->clients[n-1]);
        s->clients[n-1] = s->clients[n];
        s->clientfds[n-1] = s->clientfds[n];
    }
    s->clients[n] = 0;
    s->connected--;
    printf("Client %d removed\n", i);
}

char *fkml_recv(fkml_server *s, int c)
{
    char *buf = malloc(sizeof(char)*MAXLEN);
    fgets(buf, MAXLEN-1, s->clients[c]);

    return buf;
}

int fkml_puts(fkml_server *s, int c, char *msg)
{
    return cputs(msg, s->clientfds[c], s->clients[c]);
}

int fkml_process(fkml_server *s, int c, char *msg)
{
    int cnum = INVALID;
    msg[strlen(msg)-1] = 0;
    char *command = msg;

    if (command == strstr(command, client_command[LOGIN])) {
        cnum = LOGIN;
        char *name = strstr(command, " ") + 1;
        printf("Client %d is now known as %s\n", c, name);
        fkml_puts(s, c, server_command[ACK]);
    } else if (command == strstr(command, client_command[PLAY])) {
        cnum = PLAY;
        int card = atoi(strstr(command, " ") + 1);
        printf("Client %d played card %d\n", c, card);
        if (card > 0 && card < 60)
            fkml_puts(s, c, server_command[ACK]);
        else
            fkml_puts(s, c, server_command[FAIL]);
    } else if (command == strstr(command, client_command[MSG])) {
        cnum = MSG;
        int i;
        for (i = 0; i < s->connected; i++)
            if (i!=c) {
                fkml_puts(s, i, server_command[MSGFROM]);
                fkml_puts(s, i, "7 hello world\n");
            }
    } else if (command == strstr(command, client_command[LOGOUT])) {
        cnum = LOGOUT;
        fkml_puts(s, c, server_command[ACK]);
        fkml_puts(s, c, "\n");
        fkml_rmclient(s, c);
        int i;
        for (i = 0; i < s->connected; i++) {
            fkml_puts(s, i, server_command[MSGFROM]);
            fkml_puts(s, i, "c disconnect: bye\n");
        }
    } else
        fkml_puts(s, c, server_command[FAIL]);

    fkml_puts(s, c, "\n");

    return cnum;
}

int fkml_handle_command(fkml_server *s, char *cmd, char *args)
{
    return 0;
}

void fkml_printclients(fkml_server *s)
{
    int i;
    for (i = 0; i < MAX_PLAYERS; i++) {
        printf("Client %d: fd = %d, FILE* = %p\n", i, s->clientfds[i],
                s->clients[i]);
    }
}

void fkml_shutdown(fkml_server *s)
{
    int i;

    for (i = 0; i < s->connected; i++) {
        fkml_puts(s, i, server_command[TERMINATE]);
        fkml_puts(s, i, "\n");
        fclose(s->clients[i]);
    }

    close(s->socket);

    free(s);
}

/* vim: set sw=4 ts=4 fdm=syntax: */
