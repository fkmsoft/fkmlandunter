/* server.c
 *
 * Mainfunktion des Fkmlandunter
 * "Servers"
 *
 * fkmlandunter prtcl:
 * -----------------------------
 * -> LOGIN name
 * <- ACK nam
 * -> START
 * <- START 3 nam1 nam2 nam3
 *
 * <- WEATHER 7 8
 * <- DECK 1 2 56
 * <- RINGS 2 4 1
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
    create_players(s->players, PNUM);

    for (i = 0; i < PNUM;)
        if (fkml_addplayer(s))
            i++;

    /* This loop lasts one round */
    for (i = 0; i < s->connected; i++) {
        int *buf = shuffle(12);
        for (j = 0; j < 12; j++)
            s->water[j] = buf[j];
        free(buf);

        int alive = s->connected;
        for (p = 0; p < s->connected; p++)
            show_startmsg(s, p);

        /* This loop lasts one move */    
        for (j = 0; j < 12 && alive > 2; j++) {
            /* w_min & w_max are the current watercards */
            int w_min, w_max;
            w_min = (s->water[j] < s->water[j+1] ?
                  s->water[j] : s->water[j+1]);
            w_max = (s->water[j] > s->water[j+1] ?
                  s->water[j] : s->water[j+1]);

            /* This loop does the in- and output for every player */
            int max=0, sec=0, sec_p=-1, max_p=-1;
            for (p = 0; p < s->connected; p++) {
                print_deck(s, p);
                show_rings(s, p);
                show_weather(w_min, w_max, s, p);
        
                int w_card = read_weather(s, p);
                /* remove chosen card from deck */
                int d = 0;
                for (; s->players[p].current_deck.weathercards[d] != w_card; d++)
                    ; /* sehr nais! */
                s->players[p].current_deck.weathercards[d] = 0;
        
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
            s->players[max_p].water_level = w_min;
            s->players[sec_p].water_level = w_max;

            /* find highest waterlevel */
            max = -1;
            for (p = 0; p < s->connected; p++)
                if (s->players[p].water_level > max)
                    max = s->players[p].water_level;

            for (p = 0; p < s->connected; p++) {
                show_waterlevels(s, p);
                if (s->players[p].water_level == max)
                    if (--s->players[p].current_deck.lifebelts < 0) {
                        s->players[p].dead = true;
                        alive--;
                    }
            }
        }

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

    }

    fkml_shutdown(s);

    return 0;
}

/* Für später zum Töten:
Spieleranzahl S von Kommandozeile auswerten, sonst 3;
S Decks erstellen;
S Spieler mit Punktzahl 0 erstellen;
for i = 0 to S
    Wasserstaende 0 setzen;
    Decks zuweisen;
    Den Spielern Decks geben;    // Kommunikation
    Spieler auf lebendig setzen;
        Wasserstapel mischen;
    for j = 0 to 12
        Den Spielern neue Wasserkarten zeigen;    // Kommunikation
        Eingabe von Spielern lesen solange bis gueltig;
                            // Kommunikation
        Wasserstand der Spieler updaten;
        Spieler mit hoechstem Wasserstand ermitteln und Ring
            entfernen;
        Spielern neue Wasserstaende mitteilen; // Kommunikation
        Pruefen auf Wasserstand -1 - Spieler als Tot markieren;
        Pruefen ob Anzahl lebender Spieler >= 2
            wenn nicht Runde beenden;

    Spieler mit niedrigstem Wasserstand ermitteln
        +1 Punkt;
    Wasserstand auf Punktzahl addieren;
    Spielern Punktzahlen mitteilen; // evtl unnoetig ausser f. Markierung

Platzierung ermitteln;
Spielern Platzierung mitteilen;    // Kommunikation
*/

/* vim: set sw=4 ts=4 fdm=syntax: */
