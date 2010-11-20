/* communication.c
 *
 * Communication functions for Fkmlandunter
 *
 * (c) 2010, Fkmsoft
 */

#include "communication.h"

enum SERVER_COMMAND {
    ACK, START, WEATHER, RINGS, DECK,
    FAIL, WLEVELS, POINTS, TERMINATE,
    MSGFROM, JOIN, LEAVE, PLAYED };

enum CLIENT_COMMAND { LOGIN, START_C, PLAY, MSG, LOGOUT, INVALID };

static char *server_command[] = {
    "ACK", "START", "WEATHER", "RINGS", "DECK",
    "FAIL", "WLEVELS", "POINTS", "TERMINATE",
    "MSGFROM", "JOIN", "LEAVE", "PLAYED" };

static char *client_command[] = { "LOGIN", "START", "PLAY", "MSG", "LOGOUT" };

static enum CLIENT_COMMAND get_client_cmd(char *s);
static char *build_cmd(enum SERVER_COMMAND cmd, char *msg, ...);

void send_startmsgs(fkmserver *s)
{
    int i, pnum = s->connections;
    char buf[MAXMSGLEN];
    char *ptr = buf;
    player *p;
    for (i = 0; i < pnum; i++) {
        p = fkmserver_getc(s, i);
        ptr += sprintf(ptr, " %s", p->name);
    }

    ptr = build_cmd(START, "%d%s", pnum, buf);

    for (i = 0; i < pnum; i++)
        fkmserver_cidxsend(s, i, ptr);

    return;
}

void send_decks(fkmserver *s)
{
    int i, pnum = s->connections;
    char *msg;
    player *p;
    for (i = 0; i < pnum; i++) {
        p = fkmserver_getc(s, i);
        msg = build_cmd(DECK, "%d %d %d %d %d %d %d %d %d %d %d %d",
                p->current_deck.weathercards[0],
                p->current_deck.weathercards[1],
                p->current_deck.weathercards[2],
                p->current_deck.weathercards[3],
                p->current_deck.weathercards[4],
                p->current_deck.weathercards[5],
                p->current_deck.weathercards[6],
                p->current_deck.weathercards[7],
                p->current_deck.weathercards[8],
                p->current_deck.weathercards[9],
                p->current_deck.weathercards[10],
                p->current_deck.weathercards[11]);
        fkmserver_cidxsend(s, i, msg);
    }
}

void send_lifebelts(fkmserver *s)
{
    int i, pnum = s->connections;
    char buf[MAXMSGLEN], *ptr = buf;
    player *p;
    for (i = 0; i < pnum; i++) {
        p = fkmserver_getc(s, i);
        ptr += sprintf(ptr, " %d", p->current_deck.lifebelts);
    }
    ptr = build_cmd(RINGS, buf + 1);
    for (i = 0; i < pnum; i++)
        fkmserver_cidxsend(s, i, ptr);

    return;
}

void send_weather(fkmserver *s, int min, int max)
{
    int i, pnum = s->connections;
    char *cmd = build_cmd(WEATHER, "%d %d", min, max);

    for (i = 0; i < pnum; i++)
        fkmserver_cidxsend(s, i, cmd);

    return;
}

void send_wlevels(fkmserver *s)
{
    int i, pnum = s->connections;
    char buf[MAXMSGLEN], *ptr = buf;
    player *p;
    for (i = 0; i < pnum; i++) {
        p = fkmserver_getc(s, i);
        if (!p->dead)
            ptr += sprintf(ptr, " %d", p->water_level);
        else
            ptr += sprintf(ptr, " %d", -1);
    }
    ptr = build_cmd(WLEVELS, buf + 1);
    for (i = 0; i < pnum; i++)
        fkmserver_cidxsend(s, i, ptr);

    return;
}

void send_points(fkmserver *s)
{
    int i, pnum = s->connections;
    char buf[MAXMSGLEN], *ptr = buf;
    player *p;
    for (i = 0; i < pnum; i++) {
        p = fkmserver_getc(s, i);
        ptr += sprintf(ptr, " %d", p->points);
    }
    ptr = build_cmd(POINTS, buf + 1);
    for (i = 0; i < pnum; i++)
        fkmserver_cidxsend(s, i, ptr);

    return;
}

/* returns played weather card or -2 on disconnect, -1 if other command */
int parse_game_input(fkmserver *s, int p)
{
    bool indeck = false;
    int i, c = -1;
    player *pl = fkmserver_getc(s, p);
    char *buf = fkmserver_cidxrecv(s, p);
    trim(buf, "\r\n");
    if (strchr(buf, '%')) {
        fkmserver_cidxsend(s, p, build_cmd(FAIL,
                    "invalid character \'%%\' in input"));
        return -1;
    }

    char *cmddata = strchr(buf, ' ');
    switch(get_client_cmd(buf)) {
        case PLAY:
            if (pl->played) {
                fkmserver_cidxsend(s, p, build_cmd(FAIL,
                            "you made your move"));
            } else if (pl->dead) {
                fkmserver_cidxsend(s, p, build_cmd(FAIL,
                            "you drowned"));
            } else {
                char *cardstr = strchr(buf, ' ');
                if (!cardstr || *(++cardstr) == 0) {
                    fkmserver_cidxsend(s, p, build_cmd(FAIL,
                            "card argument missing"));
                    break;
                } else {
                    c = atoi(cardstr);
                    for (i = 0; i < 12; i++)
                        if (c > 0 &&
                                (c ==
                                pl->current_deck.weathercards[i])) {
                            indeck = true;
                            break;
                        }
                }
                if (!indeck) {
                    fkmserver_cidxsend(s, p, build_cmd(FAIL, "not in deck: %d", c));
                    c = -1;
                } else {
                    pl->played = c;
                    fkmserver_cidxsend(s, p, build_cmd(ACK, 0));
                }
            }
            break;
        case LOGIN:
            fkmserver_cidxsend(s, p, build_cmd(FAIL, "already logged in"));
            break;
        case LOGOUT:
            fkmserver_cidxsend(s, p, build_cmd(TERMINATE, "bye"));
            printf("Client %d (%s) logged out\n",
                    p, pl->name);
            send_leave(s, p);
            rmplayer(s, p);
			return -2;
        case MSG:
            if (!cmddata || *(++cmddata) == 0)
                fkmserver_cidxsend(s, p, build_cmd(FAIL, "Message expected"));
            else
                send_message(s, p, cmddata);
            break;
        default:
            fkmserver_cidxsend(s, p, build_cmd(FAIL, "Invalid command"));
    }

    return c;
}

int parse_pre_game_input(fkmserver *s, int p)
{
    int i;
    player *p2, *pl = fkmserver_getc(s, p);
    char *in = fkmserver_cidxrecv(s, p);

    trim(in, "\r\n");
    if (strchr(in, '%')) {
        if (pl->name) {
            fkmserver_cidxsend(s, p, build_cmd(FAIL,
                        "invalid character \'%%\' in input"));
        } else {
            fkmserver_cidxsend(s, p, build_cmd(TERMINATE,
                        "invalid character \'%%\' in input"));
            fkmserver_rmidxc(s, p);
        }
        return 0;
    }
    char *data = strchr(in, ' ');
    if (pl->name) { /* already logged in */
        switch(get_client_cmd(in)) {
            case MSG:
                if (!data || *(++data) == 0)
                    fkmserver_cidxsend(s, p, build_cmd(FAIL, "Message expected"));
                else
                    send_message(s, p, data);
                break;
            case START_C:
                return 3;
            case LOGOUT:
                fkmserver_cidxsend(s, p, build_cmd(TERMINATE, "bye"));
                printf("Client %d (%s) logged out\n",
                        p, pl->name);
                send_leave(s, p);
                rmplayer(s, p);
            case PLAY:
                fkmserver_cidxsend(s, p, build_cmd(FAIL, "game hasn't started"));
                break;
            case LOGIN:
                fkmserver_cidxsend(s, p, build_cmd(FAIL, "already logged in"));
                break;
            default:
                fkmserver_cidxsend(s, p, build_cmd(FAIL, "Invalid command"));
        }
    } else { /* not yet logged in */
        if (get_client_cmd(in) != LOGIN) {
            fkmserver_cidxsend(s, p, build_cmd(TERMINATE, "must log in"));
            fkmserver_rmidxc(s, p);
        } else if (!data || *(++data) == 0) {
            fkmserver_cidxsend(s, p, build_cmd(TERMINATE,
                        "nickname expected"));
            fkmserver_rmidxc(s, p);
        } else if (strchr(data, ' ')) {
            fkmserver_cidxsend(s, p, build_cmd(TERMINATE,
                        "spaces are not allowed in nicknames"));
            fkmserver_rmidxc(s, p);
        } else {
            /* check whether this name is already taken */
            for (i = 0; i < s->connections; i++) {
				p2 = fkmserver_getc(s, i);
				if (p2->name && strcmp(p2->name, data) == 0) {
					fkmserver_cidxsend(s, p, build_cmd(TERMINATE,
							  "nickname taken: `%s'", p2->name));
					fkmserver_rmidxc(s, p);
					return 0;
			    }
            }

			/* everything OK, let him in */
            pl->name = malloc(strlen(data));
            strcpy(pl->name, data);
            fkmserver_cidxsend(s, p, build_cmd(ACK, 0));
            send_join(s, p);
            return 1;
        }
    }

    return 0;
}

void send_join(fkmserver *s, int joiner)
{
    int i, pnum = s->connections;
    player *p = fkmserver_getc(s, joiner);
    char *msg = build_cmd(JOIN, p->name); 
    for (i = 0; i < pnum; i++)
        if (i != joiner)
            fkmserver_cidxsend(s, i, msg);

    return;
}

void send_leave(fkmserver *s, int leaver)
{
    int i, pnum = s->connections;
    player *p = fkmserver_getc(s, leaver);
    char *msg = build_cmd(LEAVE, p->name); 
    for (i = 0; i < pnum; i++)
        if (i != leaver)
            fkmserver_cidxsend(s, i, msg);

    return;
}

void send_played(fkmserver *s)
{
    int i, pnum = s->connections;
    player *p;
    char buf[MAXMSGLEN], *ptr = buf;
    for (i = 0; i < pnum; i++) {
        p = fkmserver_getc(s, i);
        ptr += sprintf(ptr, " %d", p->played);
    }
    ptr = build_cmd(PLAYED, buf + 1);
    for (i = 0; i < pnum; i++)
        fkmserver_cidxsend(s, i, ptr);

    return;
}

void send_message(fkmserver *s, int author, char *msg)
{
    int i, pnum = s->connections;
    player *p = fkmserver_getc(s, author);
    char *cmd = build_cmd(MSGFROM, "%s %s", p->name, msg); 
    for (i = 0; i < pnum; i++)
        if (i != author)
            fkmserver_cidxsend(s, i, cmd);

    return;
}

void send_op_message(fkmserver *s, char *msg)
{
    int i, pnum = s->connections;
    char *cmd = build_cmd(MSGFROM, "Operator %s", msg); 
    for (i = 0; i < pnum; i++)
        fkmserver_cidxsend(s, i, cmd);

    return;
}

void rmplayer(fkmserver *s, int p)
{
    int i, pnum = s->connections;
    player *pl = fkmserver_getc(s, p);
    if (pl->name) {
        free(pl->name);
        for (i = 0; i < pnum; i++)
            if (i != p)
                send_leave(s, p);
    }
    fkmserver_rmidxc(s, p);

    return;
}

void send_refuse(fkmserver *s)
{
    char *ptr = build_cmd(TERMINATE, "Server full");
    fkmserver_refusenetc(s, ptr);
}

void disconnect_fuckers(fkmserver *s, bool all)
{
    int i, pnum = s->connections;
    player *p;
    if (all) {
        for (i = 0; i < pnum; i++) {
            fkmserver_cidxsend(s, i, build_cmd(TERMINATE, "game ended"));
            p = fkmserver_getc(s, i);
            if (p->name)
                free(p->name);
        }
        for (i = 0; i < pnum; i++)
            /* always take the first, because the ones before
             * are already gone */
            fkmserver_rmidxc(s, 0);
    } else {
        for (i = 0; i < pnum; i++) {
            p = fkmserver_getc(s, i);
            if (!p->name) {
                fkmserver_rmidxc(s, i);
                i--; pnum--;
            }
        }
	}

    return;
}

static enum CLIENT_COMMAND get_client_cmd(char *s)
{
    int i;
    for (i = 0; i < INVALID; i++)
        if (strncmp(s, client_command[i], strlen(client_command[i])) == 0)
            return i;

    return INVALID;
}

static char *build_cmd(enum SERVER_COMMAND cmd, char *msg, ...)
{
    static char msgbuf[MAXMSGLEN];

    int count = sprintf(msgbuf, "%s ", server_command[cmd]);
    char *p = msgbuf + count;

    va_list ap;
    va_start(ap, msg);
    count += vsnprintf(p, MAXMSGLEN - (count + 1), msg, ap);
    msgbuf[count++] = MSGDELIM;
    msgbuf[count] = 0;

    return msgbuf;
}

/* vim: set sw=4 ts=4 fdm=syntax: */
