#/* communication.c
 *
 * Die Kommunikationsschnittstelle von
 * Fkmlandunter
 *
 * (c) Fkmsoft, 2010
 */

#include <stdio.h>
#include <ctype.h>
#include "communication.h"

#define	prompt(P)	(printf("(%s)> ", P->name))

static int _n; //XXX crap!

#if 0
/* test */
main(int argc, char *argv[])
{
    char *testnames[] = { "Hans", "Peter" };

    deck testdeck;
    int i;
    for (i = 1; i <= 12; i++)
        testdeck.weathercards[i-1] = i*2;
    testdeck.lifebelts = 25;

    player testplayer;
    testplayer.name = "Paul";
    testplayer.current_deck = testdeck;

    int testlevels[] = { 5, -1 };
    int testpoints[] = { 99, 0 };

    show_names(2, testnames, &testplayer);
    give_deck(&testplayer);
    show_waterlevel(1, 12, &testplayer);
    //printf("%d\n", get_weather(&testplayer));
    show_waterlevels(testlevels, &testplayer);
    show_points(testpoints, &testplayer);
}
#endif

/* nichts oder namen ausgeben */
void show_names(int n, char **names, player *p)
{
    prompt(p);
    _n = n; //XXX crap!
    while (n-- > 0) {
        printf("%s", *names++);
        if (n >= 1)
            printf(", ");
    }
    printf("\n");
}

/* BESCHISSENDSTE FUNKTION ÜBERHAUPT! WEGEN CRAP DESIGN NOTWENDIG */
void anzahl_der_player(int num)
{
    _n = num;
}

void print_player(player *p)
{
  printf("Player \"%s\":\n"
      "Points: %d\n", p->name, p->points);
}

/* 12 weathercards + lifebelt ausgeben */
void print_deck(player *p)
{
    prompt(p);

    int i;
    printf("Your current weathercards are: ");
    for (i = 0; i < 12; i++) {
        printf("%d", p->current_deck.weathercards[i]);
        if (i < 11)
            printf(", ");
    }
    printf("\n");
    prompt(p);
    printf("Your amount of lifebelts is: %d\n", p->current_deck.lifebelts);
}

/* Wasserstand a und b ausgeben */
void show_waterlevel(int a, int b, player *p)
{
    prompt(p);
    printf("The new \"watercards\" are %d and %d\n", a, b); 
}

/* Liest die auszuspielende Wassergarte ein */
int get_weather(player *p)
{
    bool indeck = false;
    int i, c;
    while (!indeck) {
        prompt(p);
        printf("Please enter your chosen \"watercard\": ");
        c = getint();
        for (i = 0; i < 12; i++) {
            if (c == p->current_deck.weathercards[i])
                indeck = true;
        }
    }
    return c;
}

/* Gibt alle Wasserstände aus */
void show_waterlevels(int *levels, player *p)
{
    int i;
    prompt(p);
    printf("The \"waterlevels\" are: ");
    for (i = 0; i < _n; i++) { //XXX crap!
        printf("%d", levels[i]);
        if (i < _n - 1)
            printf(", ");
    }
    printf("\n");
}
	// S Wasserstande (mit namen) nach stdout

/* Gibt alle Punkte aus */
void show_points(int *points, player *p)
{
    int i;
    prompt(p);
    printf("The score is: ");
    for (i = 0; i < _n; i++) { //XXX crap!
        printf("%d", points[i]);
        if (i < _n - 1)
            printf(", ");
    }
    printf("\n");
}

/* Hilfsmethode zum Einlesen einer Ganzzahl */
int getint()
{
    int c, n = 0;

    while((c = getchar()) != '\n')
        if(isdigit(c))
            n = 10 * n + (c - '0');

    return n;
}

