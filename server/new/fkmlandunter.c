/* fkmlandunter.c
 *
 * The implementation of the Fkmlandunter game.
 *
 * (c) 2010, Fkmsoft
 */

#include "fkmlandunter.h"

void set_alive(fkmserver *s, int playerlimit);
void set_notplayed(fkmserver *s, int playerlimit);
void set_wlevel(fkmserver *s, int p, int level);
int get_wlevel(fkmserver *s, int p);
void rm_wcard(fkmserver *s, int p, int card);
int rm_lifebelt(fkmserver *s, int p);

bool fkmlandunter_play(fkmserver *s, int playerlimit)
{
    if (!s || playerlimit < 3 || playerlimit > 5)
        return false;

    if (s->connections) {
        puts("Players on server!\n");
        return false;
    }

    int i, j, p;
    player *pl;
    int water[24];

    /* connect enough players */
    for (i = 0; i < playerlimit;) {
        p = fkmserver_poll(s);
        if (p == 0) /* new client connect */
            fkmserver_addnetc(s, (void *)create_player);
        else if (p > 0) {
            if ((j = parse_pre_game_input(s, --p)) == 3) {
                if (i > 2)
                    i = playerlimit;
            } else
                i += j;
        } else { /* p < 0, client disconnect */
            p = -p;
            rmplayer(s, --p);
        }
    }

    /* disconnect all players that aren't logged in */
    disconnect_fuckers(s, false);
    int pnum = s->connections;
    deck *deck_set = create_decks(pnum);

    /* show startmessages (only once per game) */
    send_startmsgs(s);

    /* This loop lasts one round */
    for (i = 0; i < pnum; i++) {
        /* shuffle watercards */
        int *buf = shuffle(24);
        for (j = 0; j < 24; j++)
            water[j] = (buf[j] % 12) + 1;
        free(buf);

        /* hand out decks after rotating them (using the original deckset as
         * basis */
        deck *df;
        df = deck_rotate(deck_set, i, pnum);
        pl = fkmserver_getc(s, 0);
        for (p = 0; p < pnum; p++) {
            pl = fkmserver_getc(s, p);
            pl->current_deck = df[p];
        }
        /* remove used decks (real ones are saved in deck_set) */
        free(df);

        /* make sure all players are alive again */
        int alive = pnum;
        set_alive(s, pnum);

        /* This loop lasts one move */    
        for (j = 0; j < 12 && alive > 2; j++) {
            /* w_min & w_max are the current watercards */
            int w_min, w_max;
            w_min = (water[j] < water[j+1] ?
                  water[j] : water[j+1]);
            w_max = (water[j] > water[j+1] ?
                  water[j] : water[j+1]);

            /* This loop provides all players with information necessary to
             * make their move */
            send_decks(s);
            send_lifebelts(s);
            send_weather(s, w_min, w_max);
            /* allow all players to make a move */
            set_notplayed(s, pnum);

            /* This loop gets the next card to be played from every player */
            int max = 0, sec = 0, sec_p = -1, max_p = -1;
            int played = 0;
            while (played < alive) {
                p = fkmserver_poll(s);
                if (p == 0) {
                    send_refuse(s);
                    continue;
                } else if (p < 0) {
                    p = -p;
                    rmplayer(s, --p);
                    /* FIXME */
                    send_op_message(s, "Some fucker left, game must be cancelled");
                    disconnect_fuckers(s, true);
                    return false;
                }

                int w_card;
                if ((w_card = parse_game_input(s, --p)) < 0)
                    continue;

                /* remove used weathercard from deck */
                rm_wcard(s, p, w_card);
                played++;
        
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
            /* fkmserver_getc(s, max_p)->water_level = w_min;
            fkmserver_getc(s, sec_p)->water_level = w_max; */
            set_wlevel(s, max_p, w_min);
            set_wlevel(s, sec_p, w_max);

            /* find highest waterlevel */
            max = -1;
            pl = fkmserver_getc(s, 0);
            for (p = 0; p < pnum; p++) {
                pl = fkmserver_getc(s, p);
                if (pl->water_level > max)
                    max = pl->water_level;
            }

            /* inform players about the new waterlevels & the cards everybody
             * played */
            send_wlevels(s);
            send_played(s);
            pl = fkmserver_getc(s, 0);
            for (p = 0; p < pnum; p++) {
                pl = fkmserver_getc(s, p);
                /* remove rings and make sure they die when necessary */
                if (pl->water_level == max)
                    if (--(pl->current_deck.lifebelts) < 0) {
                        pl->dead = true;
                        pl->water_level = -2;
                        alive--;
                    }
            }
        } /* one move */

        /* find lowest waterlevel: */
        int w_min = 13;
        pl = fkmserver_getc(s, 0);
        for (p = 0; p < pnum; p++) {
            pl = fkmserver_getc(s, p);
            if (!(pl->dead) && pl->water_level < w_min)
                w_min = pl->water_level;
        }

        /* distribute points */
        pl = fkmserver_getc(s, 0);
        for (p = 0; p < pnum; p++) {
            pl = fkmserver_getc(s, p);
            if (pl->dead)
                (pl->points)--;
            else {
                /* extra points for lowest waterlevel: */
                if (pl->water_level == w_min)
                     (pl->points)++;
                /* points for lifebelts: */
                pl->points += pl->current_deck.lifebelts;
            }
        }

        /* send points */
        send_points(s);

    } /* one round */

    free(deck_set);
    disconnect_fuckers(s, true);

    return true;
}

void set_alive(fkmserver *s, int pnum)
{
    int i;
    player *p;
    for (i = 0; i < pnum; i++) {
        p = fkmserver_getc(s, i);
        p->dead = false;
    }

    return;
}

void set_notplayed(fkmserver *s, int pnum)
{
    int i;
    player *p;
    for (i = 0; i < pnum; i++) {
        p = fkmserver_getc(s, i);
        p->played = 0;
    }

    return;
}

void set_wlevel(fkmserver *s, int p, int level)
{
    player *pl = fkmserver_getc(s, p);
    pl->water_level = level;

    return;
}

int get_wlevel(fkmserver *s, int p)
{
    player *pl = fkmserver_getc(s, p);
    return pl->water_level;
}

void rm_wcard(fkmserver *s, int p, int card)
{
    int i = 0;
    player *pl = fkmserver_getc(s, p);

    while(pl->current_deck.weathercards[i] != card)
        i++;

    pl->current_deck.weathercards[i] = 0;

    return;
}

/* vim: set sw=4 ts=4 fdm=syntax: */
