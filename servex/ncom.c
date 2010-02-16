/* ncom.c
 *
 * (c) fkmsft, 2010
 */

#include <stdio.h>
#include <ctype.h>

#include "ncom.h"
#include "fkml_server.h"

static int getint(FILE *fp)
{
    int c, n = 0;

    while((c = fgetc(fp)) != '\n')
        if(isdigit(c))
            n = 10 * n + (c - '0');

    return n;
}

void show_startmsg(fkml_server *s, int p)
{
    fkml_printf(s, p, "START %d", s->connected);
    int i;
    for (i = 0; i < s->connected; i++)
        fkml_printf(s, p, " %s", s->players[i].name);

    fkml_printf(s, p, "\n");
}

void print_deck(fkml_server *s, int p)
{
    fkml_printf(s, p, "DECK");
    int i;
    for (i = 0; i < 12; i++)
        if (s->players[p].current_deck.weathercards[i])
            fkml_printf(s, p, " %d", s->players[p].current_deck.weathercards[i]);
    fkml_printf(s, p, "\n");
}

void show_rings(fkml_server *s, int p)
{
    fkml_printf(s, p, "RINGS");
    int i;
    for (i = 0; i < s->connected; i++)
        fkml_printf(s, p, " %d", s->players[i].current_deck.lifebelts);
    fkml_printf(s, p, "\n");
}

void show_weather(int min, int max, fkml_server *s, int p)
{
    fkml_printf(s, p, "WEATHER %d %d\n", min, max);
}

void show_waterlevels(fkml_server *s, int p)
{
    fkml_printf(s, p, "WLEVELS");
    int i;
    for (i = 0; i < s->connected; i++)
        if (!s->players[i].dead)
            fkml_printf(s, p, " %d", s->players[i].water_level);

    fkml_printf(s, p, "\n");
}

void show_points(fkml_server *s, int p)
{
    fkml_printf(s, p, "POINTS");
    int i;
    for (i = 0; i < s->connected; i++)
        fkml_printf(s, p, " %d", s->players[p].points);

    fkml_printf(s, p, "\n");
}

int read_weather(fkml_server *s, int p)
{
    bool indeck = false;
    int i, c;
    while (!indeck) {
        c = getint(s->players[p].fp);
        for (i = 0; i < 12; i++)
            if (c > 0 && c == s->players[p].current_deck.weathercards[i]) {
                indeck = true;
                break;
            }

        if (!indeck)
            fkml_printf(s, p, "FAIL\n");
    }

    fkml_printf(s, p, "ACK %d\n", c);
    return c;
}

/* vim: set sw=4 ts=4 fdm=syntax: */
