/* ncom.c
 *
 * (c) fkmsft, 2010
 */

#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "ncom.h"
#include "fkml_server.h"

/*static int getint(FILE *fp)
{
    int c, n = 0;

    while((c = fgetc(fp)) != '\n')
        if(isdigit(c))
            n = 10 * n + (c - '0');

    return n;
}*/

void show_startmsg(fkml_server *s, int p)
{
    int i;
    char buf[MAXLEN];
    char *ptr = buf;
    for (i = 0; i < s->connected; i++)
        ptr += sprintf(ptr, " %s", s->players[i].name);

    send_cmd(s, p, START, "%d%s", s->connected, buf);
}

void print_deck(fkml_server *s, int p)
{
    send_cmd(s, p, DECK, "%d %d %d %d %d %d %d %d %d %d %d %d",
            s->players[p].current_deck.weathercards[0],
            s->players[p].current_deck.weathercards[1],
            s->players[p].current_deck.weathercards[2],
            s->players[p].current_deck.weathercards[3],
            s->players[p].current_deck.weathercards[4],
            s->players[p].current_deck.weathercards[5],
            s->players[p].current_deck.weathercards[6],
            s->players[p].current_deck.weathercards[7],
            s->players[p].current_deck.weathercards[8],
            s->players[p].current_deck.weathercards[9],
            s->players[p].current_deck.weathercards[10],
            s->players[p].current_deck.weathercards[11]);
}

void show_rings(fkml_server *s, int p)
{
    int i;
    char buf[MAXLEN], *ptr = buf;
    for (i = 0; i < s->connected; i++)
        ptr += sprintf(ptr, " %d", s->players[i].current_deck.lifebelts);
    send_cmd(s, p, RINGS, buf);
}

void show_weather(int min, int max, fkml_server *s, int p)
{
    send_cmd(s, p, WEATHER, "%d %d", min, max);
}

void show_waterlevels(fkml_server *s, int p)
{
    int i;
    char buf[MAXLEN], *ptr = buf;
    for (i = 0; i < s->connected; i++)
        if (!s->players[i].dead)
            ptr += sprintf(ptr, " %d", s->players[i].water_level);
    send_cmd(s, p, WLEVELS, buf);
}

void show_points(fkml_server *s, int p)
{
    int i;
    char buf[MAXLEN], *ptr = buf;
    for (i = 0; i < s->connected; i++)
        ptr += sprintf(ptr, " %d", s->players[p].points);
    send_cmd(s, p, POINTS, buf);
}

int read_weather(fkml_server *s, int p)
{
    bool indeck = false;
    int i, c;
    char buf[BUFSIZ];
    while (!indeck) {
        fgets(buf, BUFSIZ-1, s->players[p].fp);

        switch(get_client_cmd(buf)) {
            case PLAY:
                sscanf(strchr(buf, ' '), "%d\n", &c);
                for (i = 0; i < 12; i++)
                    if (c > 0 &&
                            c == s->players[p].current_deck.weathercards[i]) {
                        indeck = true;
                        break;
                    }
                if (!indeck)
                    send_cmd(s, p, FAIL, "%d", c);
                break;
            case LOGIN:
                send_cmd(s, p, FAIL, "already logged in");
                break;
            case LOGOUT:
                send_cmd(s, p, TERMINATE, "bye");
                printf("Client %d (%s) disconnected!\n",
                        p, s->players[p].name);
                fkml_rmclient(s, p);
                break;
            case MSG:
                trim(buf, "\r\n");
                for (i = 0; i < s->connected; i++)
                    if (i != p) {
                        send_cmd(s, i, MSGFROM, "%s %s",
                                s->players[p].name, strchr(buf, ' ') + 1);
                    }
                break;
            default:
                send_cmd(s, p, INVALID, buf);
        }
    }

    send_cmd(s, p, ACK, "%d", c);
    return c;
}

/* vim: set sw=4 ts=4 fdm=syntax: */
