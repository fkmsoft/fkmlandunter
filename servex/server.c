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
/*#include "communication.h"*/
#include "server_util.h"
#include "servex/fkml_server.h"

#define PNUM (3)
#define PORT (1337)

int main(int argc, char **argv)
{
    int /*playernum,*/ i, j, p;
    /* player *players; */
    fkml_server *s = init_server(PORT, PNUM);
    /* int water[24]; */

    /*argc < 2 ? s->connected = 3 : (s->connected = atoi(argv[1]));*/
    printf("%d connected\n", s->connected);
    create_players(s->players, PNUM);

    for (i = 0; i < PNUM; i++)
        fkml_addplayer(s);
    /* anzahl_der_player(playernum); */

    /* This loop lasts one round */
    for (i = 0; i < s->connected; i++) {
        int *buf = shuffle(12);
        for (j = 0; j < 12; j++)
            s->water[j] = buf[j];
        free(buf);

        int alive = s->connected;

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
                /* show_waterlevel(w_min, w_max, &players[p]); */
                show_weather(w_min, w_max, s, p);
        
                /* int w_card = get_weather(&s->players[p]); */
                int w_card = read_weather(s, p);
                /* remove chosen card from deck */
                int d = 0;
                for (; s->players[p].current_deck.weathercards[d] != w_card; d++)
                    ; /* sehr nais! */
                s->players[p].current_deck.weathercards[d] = 0;
        
                /*XXX AB HIER NICHTS MEHR VERÄNDERT*/
        
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

            /* hoesten wasserstand finden */
            max = -1;
            for (p = 0; p < s->connected; p++)
                if (s->players[p].water_level > max)
                    max = s->players[p].water_level;

            /* int *wlevels = malloc(sizeof(int)*s->connected);
            if (max > 0) */
                for (p = 0; p < s->connected; p++) {
                    /* wlevels[p] = players[p].water_level; */
                    show_waterlevels(s, p);
                    if (s->players[p].water_level == max)
                        if (--s->players[p].current_deck.lifebelts < 0) {
                            s->players[p].dead = true;
                            alive--;
                        }
                }
            /* else
                printf("Etwas schreckliches ist geschehen! (%d)\n", max); */

            /* for (p=0; p < s->connected; p++) 
                show_waterlevels(wlevels, &players[p]); */
            /* free(wlevels); */
        }

        /* Wertung: */
        /* Niedrigsten Wasserstand finden: */
        int w_min = 13;
        /* int *points = malloc(sizeof(int)*playernum); */
        for (p = 0; p < s->connected; p++)
            if (s->players[p].water_level < w_min)
                w_min = s->players[p].water_level;

        /* Punkte verteilen: */
        for (p = 0; p < s->connected; p++) {
              /* Extrapunkte fuer Niedrigwasser verteilen: */
              if (s->players[p].water_level == w_min)
                  s->players[p].points++;
              /* Punkte fuer Ringe verteilen: */
              s->players[p].points += s->players[p].current_deck.lifebelts;
              /* points[p] = players[p].points; */
        }

        for (p = 0; p < s->connected; p++)
              show_points(s, p);

        /* free(points); */
    }

    /* Platzierung ermitteln */
    printf("Results:\n");
    for (p = 0; p < s->connected; p++)
        printf("%s:\t%d\n", s->players[p].name, s->players[p].points);

    fkml_shutdown(s);
    /*free(players);*/

    return 0;
}

/*
 * Für später zum Töten:
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
