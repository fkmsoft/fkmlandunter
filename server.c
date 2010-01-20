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

#include "communication.h"
#include "server_util.h"

int main(int argc, char **argv)
{
    int playernum, i, j, p;
    player *players;
	int water[24]; // rueckt weiter je bei erhoehung von turn_num

    argc < 2 ? playernum = 3 : (playernum = atoi(argv[1]));
    players = create_players(playernum);
    anzahl_der_player(playernum);

    /* This loop lasts one round */
    for (i = 0; i < playernum; i++) {
	int *buf = shuffle(12);
	for (j = 0; j < 12; j++)
	    water[j] = buf[j];
	free(buf);

	int alive = playernum;

	/* This loop lasts one move */	
	for (j = 0; j < 12 && alive > 2; j++) {
	    /* w_min & w_max are the current watercards */
	    int w_min, w_max;
	    w_min = (water[j] < water[j+1] ?
		      water[j] : water[j+1]);
	    w_max = (water[j] > water[j+1] ?
		      water[j] : water[j+1]);

	    /* This loop does the in- and output for every player */
	    int max=0, sec=0, sec_p=-1, max_p=-1;
	    for (p = 0; p < playernum; p++) {
		print_deck(&players[p]);
		show_waterlevel(w_min, w_max, &players[p]);

		int w_card = get_weather(&players[p]);
		/* remove chosen card from deck */
		int d = 0;
		for (; players[p].current_deck.weathercards[d] != w_card; d++)
		    ; // sehr nais!
		players[p].current_deck.weathercards[d] = 0;

		//XXX AB HIER NICHTS MEHR VERÄNDERT

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
	    players[max_p].water_level = w_min;
	    players[sec_p].water_level = w_max;

	    // hoesten wasserstand finden
	    max = -1;
	    for (p = 0; p < playernum; p++)
		if (players[p].water_level > max)
		    max = players[p].water_level;

	    int *wlevels = malloc(sizeof(int)*playernum);
	    if (max > 0)
		for (p = 0; p < playernum; p++) {
		    wlevels[p] = players[p].water_level;
		    if (players[p].water_level == max)
			if (--players[p].current_deck.lifebelts < 0) {
			    players[p].dead = true;
			    alive--;
			}
		}
	    else
		printf("Etwas schreckliches ist geschehen! (%d)\n", max);

	    for (p=0; p < playernum; p++)
		show_waterlevels(wlevels, &players[p]);
	    free(wlevels);
	}

    // Wertung:
    // Niedrigsten Wasserstand finden:
    int w_min = 13;
    int *points = malloc(sizeof(int)*playernum);
    for (p = 0; p < playernum; p++)
      if (players[p].water_level < w_min)
        w_min = players[p].water_level;

    // Punkte verteilen:
    for (p = 0; p < playernum; p++) {
      // Extrapunkte fuer Niedrigwasser verteilen:
      if (players[p].water_level == w_min)
        players[p].points++;
      // Punkte fuer Ringe verteilen:
      players[p].points += players[p].current_deck.lifebelts;
      points[p] = players[p].points;
    }

    for (p = 0; p < playernum; p++)
      show_points(points, &players[p]);

    free(points);
  }

  // Platzierung ermitteln
  printf("Results:\n");
  for (p = 0; p < playernum; p++)
    printf("%s:\t%d\n", players[p].name, players[p].points);

  free(players);

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
	Den Spielern Decks geben;	// Kommunikation
	Spieler auf lebendig setzen;
        Wasserstapel mischen;
	for j = 0 to 12
		Den Spielern neue Wasserkarten zeigen;	// Kommunikation
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
Spielern Platzierung mitteilen;	// Kommunikation
*/
/* vim: set sw=4 ts=4 fdm=syntax: */
