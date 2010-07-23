/* server.c
 *
 * Mainfunktion des Fkmlandunter
 * "Servers"
 *
 * (c) Fkmsoft, 2010
 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
/* #include <stdio.h> */
#include <time.h>

/* for getopt */
#include <getopt.h>

#include "ncom.h"
#include "server_util.h"
#include "fkml_server.h"

#define PORT (1337)

int main(int argc, char **argv)
{
    bool debug = false;
    int i, j, p, pnum = MAX_PLAYERS, games = 1;
    while ((i = getopt(argc, argv, "l:dg:h")) != -1) {
        switch (i) {
            case 'l':
                pnum = atoi(optarg);
                break;
            case 'd':
                debug = true;
				break;
			case 'g':
				games = atoi(optarg);
				break;
            case 'h':
                printf("usage: %s [-l playerlimit] [-g number_of_games]\n", argv[0]);
                exit(EXIT_SUCCESS);
            default:
                exit(EXIT_FAILURE);
        }
    }

	int g;
	for (g = 0; g < games; g++) {
    fkml_server *s = init_server(PORT, pnum);

    deck *deck_set = create_decks(pnum);
    create_players(s->players, pnum);

    for (i = 0; i < pnum;)
        if ((p = poll_for_input(s)) == -1) {
            if (fkml_addplayer(s)) {
                for (j = 0; j < s->connected; j++)
                    if (j != i)
                        show_join(s, j, i);
                i++;
            }
        } else if (p >= 0) {
            char buf[MAXLEN];
            fgets(buf, MAXLEN-1, s->players[p].fp);
            trim(buf, "\r\n");
            if (strchr(buf, '%')) {
                send_cmd(s, p, FAIL, "invalid character \'%%\' in input");
                continue;
            }
            char *data = strchr(buf, ' ');
            /* printf("Got input from client %d (%s)\n",
                    p, s->players[p].name); */
            switch(get_client_cmd(buf)) {
                case MSG:
                    /* printf("Sending message %s\n", buf); */
                    if (!data || *(++data) == 0) {
                        send_cmd(s, p, FAIL, "Message expected");
                    } else for (j = 0; j < s->connected; j++)
                            if (j != p)
                                send_cmd(s, j, MSGFROM, "%s %s",
                                        s->players[p].name, data);
                    /* puts("Done."); */
                    break;
                case START_C:
                    if (i > 2)
                        i = pnum;
                    else
                        send_cmd(s, p, FAIL, "not enough players");
                    break;
                case LOGOUT:
                    for (j = 0; j < s->connected; j++)
                        if (p == j)
                            send_cmd(s, p, TERMINATE, "bye");
                        else
                            show_leave(s, j, p);
                    fkml_rmclient(s, p);
                    i--;
                    break;
                case PLAY:
                case LOGIN:
                default:
                    send_cmd(s, p, FAIL, "invalid command");
            }
        }

    /* show startmessages (only once per game) */
    for (p = 0; p < s->connected; p++)
        show_startmsg(s, p);

    /* This loop lasts one round */
    for (i = 0; i < s->connected; i++) {
        /* shuffle watercards */
        int *buf = shuffle(24);
        for (j = 0; j < 24; j++)
            s->water[j] = (buf[j] % 12) + 1;
        free(buf);

        /* hand out decks after rotating them (using the original deckset as
         * basis */
        deck *df;
        hand_decks(s->players, (df = deck_rotate(deck_set, i, s->connected)),
                s->connected);
        /* remove used decks (real ones are saved in deck_set) */
        free(df);

        /* make sure all players are alive again */
        int alive = s->connected;
        for (p = 0; p < s->connected; p++) {
            s->players[p].dead = false;

            /* and tell them about the new round */
            /* show_startmsg(s, p); DONt */

            /* and reset their waterlevel */
            s->players[p].water_level = 0;
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
                show_deck(s, p);
                show_rings(s, p);
                show_weather(w_min, w_max, s, p);
                /* allow all players to make a move */
                s->players[p].played = 0;
            }

            /* This loop gets the next card to be played from every player */
            int max = 0, sec = 0, sec_p = -1, max_p = -1;
            int played = 0, plnum = s->connected;
            while (played < alive) {
                if (s->connected < plnum) {
                    fkml_shutdown(s,
                            "game aborted because of client disconnect");
                    /* puts("game aborted because of client disconnect"); */
                    return 1;
                }

                if ((p = poll_for_input(s)) < 0) {
                    if (p == -1)
                        fkml_failclient(s);
                    continue;
                }

                int w_card;
                if ((w_card = read_weather(s, p)) < 0)
                    continue;

                /* remove chosen card from deck */
                int d = 0;
                for (played++;
                        s->players[p].current_deck.weathercards[d] != w_card;
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

            /* inform players about the new waterlevels & the cards everybody
             * played */
            for (p = 0; p < s->connected; p++) {
                show_waterlevels(s, p);
                show_played(s, p);
                /* remove rings and make sure they die when necessary */
                if (s->players[p].water_level == max)
                    if (--s->players[p].current_deck.lifebelts < 0) {
                        s->players[p].dead = true;
                        s->players[p].water_level = -2;
                        alive--;
                    }
            }
        } /* one move */

        /* evolution: */
        /* find lowest waterlevel: */
        int w_min = 13;
        for (p = 0; p < s->connected; p++)
            if (!s->players[p].dead && s->players[p].water_level < w_min)
                w_min = s->players[p].water_level;

        /* distribute points */
        for (p = 0; p < s->connected; p++) {
            if (s->players[p].dead)
                s->players[p].points--;
            else {
                /* extra points for lowest waterlevel: */
                if (s->players[p].water_level == w_min)
                     s->players[p].points++;
                /* points for lifebelts: */
                if (debug && s->players[p].current_deck.lifebelts > 12)
           	        printf("player %d has LOTS of lifebelts left: %d!\n", p,
                        s->players[p].current_deck.lifebelts);
                s->players[p].points += s->players[p].current_deck.lifebelts;
            }
        }

        for (p = 0; p < s->connected; p++)
              show_points(s, p);

        /* free_decks(s); */
    } /* one round */

    fkml_shutdown(s, "thank you, play again");
    /* puts("bye"); */
	}

    return 0;
}

/* vim: set sw=4 ts=4 et fdm=syntax: */
