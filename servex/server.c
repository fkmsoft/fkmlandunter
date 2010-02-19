/* server.c
 *
 * Mainfunktion des Fkmlandunter
 * "Servers"
 *
 * (c) Fkmsoft, 2010
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "ncom.h"
#include "server_util.h"
#include "fkml_server.h"

#define PNUM (3)
#define PORT (1337)

int main(int argc, char **argv)
{
    int i, j, p;
    fkml_server *s = init_server(PORT, PNUM);

    printf("%d connected\n", s->connected);
    deck *deck_set = create_decks(PNUM);
    create_players(s->players, PNUM);

    for (i = 0; i < PNUM;)
        if (fkml_addplayer(s))
            i++;

    /* show startmessages (only once per game) */
    for (p = 0; p < PNUM; p++)
        show_startmsg(s, p);

    /* This loop lasts one round */
    for (i = 0; i < s->connected; i++) {
        /* shuffle watercards */
        int *buf = shuffle(12);
        for (j = 0; j < 12; j++)
            s->water[j] = buf[j];
        free(buf);

        /* hand out decks after rotating them (using the original deckset as
         * basis */
        hand_decks(s->players, deck_rotate(deck_set, i, s->connected),
                s->connected);

        /* make sure all players are alive again */
        int alive = s->connected;
        for (p = 0; p < s->connected; p++) {
            s->players[p].dead = false;
            /* and tell them about the new round */
            /* show_startmsg(s, p); DONt */
        }

        /* This loop lasts one move */    
        for (j = 0; j < 12 && alive > 2; j++) {
            /* w_min & w_max are the current watercards */
            int w_min, w_max;
            w_min = (s->water[j] < s->water[j+1] ?
                  s->water[j] : s->water[j+1]);
            w_max = (s->water[j] > s->water[j+1] ?
                  s->water[j] : s->water[j+1]);

            /* This loop provides all players with information necessary to
             * make their move */
            for (p = 0; p < s->connected; p++) {
                print_deck(s, p);
                show_rings(s, p);
                show_weather(w_min, w_max, s, p);
            }

            /* This loop gets the next card to be played from every player */
            int max = 0, sec = 0, sec_p = -1, max_p = -1;
            for (p = 0; p < s->connected; p++) {
                int w_card = read_weather(s, p);
                /* remove chosen card from deck */
                int d = 0;
                for (; s->players[p].current_deck.weathercards[d] != w_card;
                        d++)
                    ; /* it's all done! */
                /* remove used weathercard from deck */
                s->players[p].current_deck.weathercards[d] = 0;
        
                /* remember highest and lowest card */
                if (w_card > max) {
                    sec = max;
                    sec_p = max_p;
                    max = w_card;
                    max_p = p;
                } else if (w_card > sec) {
                    sec = w_card;
                    sec_p = p;
                }
            }

            /* hand out new watercards */
            s->players[max_p].water_level = w_min;
            s->players[sec_p].water_level = w_max;

            /* find highest waterlevel */
            max = -1;
            for (p = 0; p < s->connected; p++)
                if (s->players[p].water_level > max)
                    max = s->players[p].water_level;

            /* inform players about the new waterlevels */
            for (p = 0; p < s->connected; p++) {
                show_waterlevels(s, p);
                /* remove rings and make sure they die when necessary */
                if (s->players[p].water_level == max)
                    if (--s->players[p].current_deck.lifebelts < 0) {
                        s->players[p].dead = true;
                        alive--;
                    }
            }
        } /* one move */

        /* evolution: */
        /* find lowest waterlevel: */
        int w_min = 13;
        for (p = 0; p < s->connected; p++)
            if (s->players[p].water_level < w_min)
                w_min = s->players[p].water_level;

        /* distribute points */
        for (p = 0; p < s->connected; p++) {
              /* extra points for lowest waterlevel: */
              if (s->players[p].water_level == w_min)
                  s->players[p].points++;
              /* points for lifebelts: */
              s->players[p].points += s->players[p].current_deck.lifebelts;
        }

        for (p = 0; p < s->connected; p++)
              show_points(s, p);

        /* remove used decks (real ones are saved in deck_set) */
        free_decks(s);
    } /* one round */

    fkml_shutdown(s);
    puts("bye");

    return 0;
}

/* vim: set sw=4 ts=4 fdm=syntax: */
