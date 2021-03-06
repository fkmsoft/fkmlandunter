/* fkml_server.c
 *
 * (c) Fkmsft, 2010
 */

/* for fdopen() from stdio */
#define _POSIX_SOURCE
/* for POLLRDHUP from poll */
#define _GNU_SOURCE

#include <stdarg.h>
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
/* for poll() */
#include <poll.h>

#include "ncom.h"
#include "fkml_server.h"

#define CMD_DELIM ('\n')

static char *server_command[] = {
    "ACK", "START", "WEATHER", "RINGS", "DECK",
    "FAIL", "WLEVELS", "POINTS", "TERMINATE",
    "MSGFROM", "JOIN", "LEAVE", "PLAYED" };

static char *client_command[] = { "LOGIN", "START", "PLAY", "MSG", "LOGOUT" };

/* returns number of client or -1 if error or need to read from master socket */
int poll_for_input(fkml_server *s)
{
    int i, ret = -1;
    struct pollfd *pfds = malloc((s->connected + 1)*sizeof(struct pollfd));

    for (i = 0; i < s->connected; i++) {
        pfds[i].fd = s->players[i].fd;
        pfds[i].events = POLLIN | POLLRDHUP;
    }
    pfds[i].fd = s->socket;
    pfds[i].events = POLLIN;

    if (poll(pfds, s->connected + 1, -1) < 0)
        perror("poll_for_input: poll failed:");

    for (i = 0; ret < 0 && i < s->connected; i++) {
        if (pfds[i].revents & POLLIN)
            ret = i;
        if (pfds[i].revents & POLLRDHUP) {
            printf("Client %d (%s) disconnected\n", i,
                    s->players[i].name);
            int p;
            for (p = 0; p < s->connected; p++)
                if (p != i)
                    show_leave(s, p, i);
            fkml_rmclient(s, i);
            ret = -2;
        }
    }

    free(pfds);

    return ret;
}

void trim(char *str, char *evil)
{
    char *ep;
    for (ep = str + strlen(str); ep > str && strchr(evil, *(ep-1)); ep--)
        ;
    *ep = 0;
}

fkml_server *init_server(unsigned int port, unsigned int players)
{
    if (players > MAX_PLAYERS || players < 0)
        players = MAX_PLAYERS;

    fkml_server *server = calloc(1,sizeof(fkml_server));

    if (port < 1024)
        printf("Please choose a port > 1024 instead of %d\n", port);

    if ((server->socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        perror("socket");

    server->connected = 0;

    struct sockaddr_in s_addr;
    memset(&s_addr, 0, sizeof(s_addr));

    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = INADDR_ANY;
    s_addr.sin_port = htons(port);

    int val = 1;
    if (setsockopt(server->socket, SOL_SOCKET, SO_REUSEADDR,
            &val, sizeof(int)) == -1)
        perror("setsockopt");

    if (bind(server->socket, (struct sockaddr *) &s_addr, sizeof(s_addr)) == -1)
        perror("bind");
    if (listen(server->socket, /*players*/1) == -1)
        perror("listen");

    /* puts("Server initialized, returning control"); */

    return server;
}

enum CLIENT_COMMAND get_client_cmd(char *s)
{
    int i;
    for (i = 0; i < 5; i++)
        if (strncmp(s, client_command[i], strlen(client_command[i])) == 0)
            return i;

    return INVALID;
}

void send_cmd(fkml_server *s, int c, enum SERVER_COMMANDS cmd, char *msg, ...)
{
    FILE *fp;
    if (!s) {
        fp = fdopen(c, "a+");
        if (setvbuf(/*s->players[s->connected].*/fp, 0, _IOLBF, 0) != 0)
            perror("setvbuf");
    } else
        fp = s->players[c].fp;

    va_list ap;
    va_start(ap, msg);
    fputs(server_command[cmd], fp);
    fputc(' ', fp);
    vfprintf(fp, msg, ap);
    fputc(CMD_DELIM, fp);
    fflush(fp);

    if (!s)
        fclose(fp);
}

int fkml_addclient(fkml_server *s, int fd)
{
    s->players[s->connected].fd = fd;
    s->players[s->connected].fp = fdopen(fd, "a+");
    if (setvbuf(s->players[s->connected].fp, 0, _IOLBF, 0) != 0)
        perror("setvbuf");
    s->connected++;
    return s->connected-1;
}

bool fkml_addplayer(fkml_server *s)
{
    int i, fd = accept(s->socket, 0, 0);

    if (fd == -1) {
        puts("Error waiting for client connection");
        return false;
    } else {
        /* printf("Adding player with fd %d\n", fd); */
        int newplayer = fkml_addclient(s, fd);

        char buf[MAXLEN];
        fgets(buf, MAXLEN-1, s->players[newplayer].fp);
        /* printf("Client said: %s", buf); */
        trim(buf,"\n\r");
        char *nam;
        if (strncmp(buf, client_command[LOGIN],
                    strlen(client_command[LOGIN])) != 0
                || (nam = strchr(buf, ' ')) == 0
                || *(++nam) == 0) {
            send_cmd(s, newplayer, FAIL, "Player name expected");
            fkml_rmclient(s, newplayer);
            return false;
        } else if (strchr(nam, '%')) {
            send_cmd(s, newplayer, FAIL,
                    "Player names must not contain \'%%\'");
            fkml_rmclient(s, newplayer);
            return false;
        } else if (strchr(nam, ' ')) {
            send_cmd(s, newplayer, FAIL,
                    "Player names must not contain spaces");
            fkml_rmclient(s, newplayer);
            return false;
        } else {
			/* check whether name is taken yet */
			for (i = 0; i < s->connected - 1; i++)
				if (strcmp(s->players[i].name, nam) == 0) {
					send_cmd(s, newplayer, FAIL, "nickname taken");
					fkml_rmclient(s, newplayer);
					return false;
				}
			
			/* everything ok, let him in */
            s->players[newplayer].name = malloc(sizeof(char) *
                    (buf + strlen(buf) - nam/*strlen(client_command[LOGIN])*/));
            strcpy(s->players[newplayer].name, nam);
            send_cmd(s, newplayer, ACK, s->players[newplayer].name);
            return true;
        }
    }
}

void fkml_rmclient(fkml_server *s, int i)
{
    if (i < 0)
        return;

    /* printf("Removing client %i\n", i); */

    /* printf("Closing fd %d\n", s->clientfds[n-1]); */
    fclose(s->players[i].fp);
    if (s->players[i].name)
        free(s->players[i].name);
    /* if (s->players[n-1].current_deck != 0)
        free(&s->players[n-1].current_deck); */

    int n;
    for (n = i+1; n < MAX_PLAYERS && s->players[n-1].fp; n++)
        s->players[n-1] = s->players[n];

    s->players[n].fp = 0;
    s->connected--;
    printf("Client %d removed\n", i);
}

char *fkml_recv(fkml_server *s, int c)
{
    char *buf = malloc(sizeof(char)*MAXLEN);
    fgets(buf, MAXLEN-1, s->players[c].fp);

    return buf;
}

int fkml_puts(fkml_server *s, int c, char *msg)
{
    return 8;
}

void fkml_printf(fkml_server *s, int c, char *fmt, ...)
{
    fputs(fmt, stdout);
    va_list ap;
    va_start(ap, fmt);
    vfprintf(s->players[c].fp, fmt, ap);
    fflush(s->players[c].fp);
    /*va_end(ap);*/
}

int fkml_process(fkml_server *s, int c, char *msg)
{
    int cnum = INVALID;
    msg[strlen(msg)-2] = 0; /* ignore \r\n */
    char *command = msg;

    if (command == strstr(command, client_command[LOGIN])) {
        cnum = LOGIN;
        s->players[c].name = malloc(sizeof(char)*
                strlen(strstr(command, " ") + 1));
        s->players[c].name = strstr(command, " ") + 1;
        printf("Client %d is now known as %s\n", c, s->players[c].name);
        fkml_printf(s, c, "%s\n", server_command[ACK]);
    } else if (command == strstr(command, client_command[START])) {
        if (s->connected > 2) {
            int i;
            for (i = 0; i < s->connected; i++)
                fkml_printf(s, i, "%s %d\n", server_command[START],
                        s->connected);
        } else
            fkml_printf(s, c, "%s Not enough players\n",
                    server_command[FAIL]);
    } else if (command == strstr(command, client_command[PLAY])) {
        cnum = PLAY;
        int card = atoi(strstr(command, " ") + 1);
        printf("Client %d played card %d\n", c, card);
        if (card > 0 && card < 60)
            fkml_printf(s, c, "%s\n", server_command[ACK]);
        else
            fkml_printf(s, c, "%s\n", server_command[FAIL]);
    } else if (command == strstr(command, client_command[MSG])) {
        cnum = MSG;
        int i;
        for (i = 0; i < s->connected; i++)
            if (i!=c) {
                fkml_printf(s, i, "%s %s %s\n", server_command[MSGFROM],
                        s->players[c].name, strstr(command, " ") + 1);
            }
    } else if (command == strstr(command, client_command[LOGOUT])) {
        cnum = LOGOUT;
        fkml_puts(s, c, server_command[ACK]);
        fkml_puts(s, c, "\n");
        fkml_rmclient(s, c);
        int i;
        for (i = 0; i < s->connected; i++) {
            fkml_printf(s, i, "%s server %s disconnected: %s\n", server_command[MSGFROM],
                    s->players[c].name, strstr(command, " ") + 1);
        }
    } else
        fkml_printf(s, c, "%s\n", server_command[FAIL]);

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
        /*print_player(&(s->players[i]));*/
    }
}

void fkml_failclient(fkml_server *s)
{
    send_cmd(0, accept(s->socket, 0, 0), FAIL, "Server full");
}

void fkml_shutdown(fkml_server *s, char *msg)
{
    int i;

    for (i = 0; i < s->connected; i++) {
        send_cmd(s, i, TERMINATE, msg);
        fclose(s->players[i].fp);
    }

    close(s->socket);

    free(s);
}

/* vim: set sw=4 ts=4 fdm=syntax: */
