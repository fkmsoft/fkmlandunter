/* communication.c
 *
 * Die Kommunikationsschnittstelle von
 * Fkmlandunter
 *
 * (c) Fkmsoft, 2010
 */

#include "communication.h"


player *create_player()
{
    /* creating deck */
    deck *d = malloc(sizeof(deck));
    int i;
    for (i = 0; i < 12; i++)
	d->weathercards[i] = 0;
    d->lifebelts = 0;

    /* creating player */
    player *p = malloc(sizeof(player));
    p->points = 0;
    p->current_deck = d;
    p->water_level = 0;
    p->dead = false;
    p->name = ""; 

    return p;
}

char *request_nick()
{
    char *nick;
    nick = malloc(MAXNICK * sizeof(char));
    write_win(GAME_BOX, "Please enter your nickname: \n");
    read_win(GAME_BOX, nick, MAXNICK);
    return nick;
}

void parse_deck(player *p, char *s)
{
    if (sscanf(s, "DECK %d %d %d %d %d %d %d %d %d %d %d %d",
	&p->current_deck->weathercards[0], &p->current_deck->weathercards[1],
        &p->current_deck->weathercards[2], &p->current_deck->weathercards[3],
        &p->current_deck->weathercards[4], &p->current_deck->weathercards[5],
        &p->current_deck->weathercards[6], &p->current_deck->weathercards[7],
	&p->current_deck->weathercards[8], &p->current_deck->weathercards[9],
	&p->current_deck->weathercards[10], &p->current_deck->weathercards[11]) < 12)
	write_win(GAME_BOX, "Error parsing deck\n");
}

void print_deck(player *p)
{
    int i;
    write_win(GAME_BOX, "Your current weathercards are:\n");
    for (i = 0; i < 12; i++) {
        if (p->current_deck->weathercards[i] > 0) {
            write_win(GAME_BOX, "%d", p->current_deck->weathercards[i]);
            if (i < 11)
                write_win(GAME_BOX, ", ");
        }
    }
    write_win(GAME_BOX, "\n");
    write_win(GAME_BOX, "Your amount of lifebelts is: %d\n", p->current_deck->lifebelts);
}

void parse_start(char *s, player **p, int *count)
{
    int i;

    sscanf(s, "START %d", count);

    /* creating count players */
    p = malloc(*count * sizeof(player));
    for (i = 0; i < *count; i++) {
	p[i]->points = 0;
	p[i]->current_deck = NULL;
	p[i]->water_level = 0;
	p[i]->dead = false;
	p[i]->name = ""; 
    }

    /* parsing names */
    for (i = 0; i < *count; i++)
	sscanf(s, "%s", p[i]->name);
}

/* Hilfsmethode zum Einlesen einer Ganzzahl */
static int getint()
{
    int c, n = 0;

    while((c = getchar()) != '\n')
        if(isdigit(c))
            n = 10 * n + (c - '0');

    return n;
}

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
#if 0
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
#endif

/* BESCHISSENDSTE FUNKTION ÜBERHAUPT! WEGEN CRAP DESIGN NOTWENDIG */
#if 0
void anzahl_der_player(int num)
{
    _n = num;
}
#endif

#if 0
void print_player(player *p)
{
    printf("Player \"%s\":\n"
        "points = %d, ", p->name, p->points);
}
#endif

/* 12 weathercards + lifebelt ausgeben */
#if 0
void print_deck(player *p)
{
    prompt(p);

    int i;
    printf("Your current weathercards are: ");
    for (i = 0; i < 12; i++) {
        if (p->current_deck.weathercards[i] > 0) {
            printf("%d", p->current_deck.weathercards[i]);
            if (i < 11)
                printf(", ");
        }
    }
    printf("\n");
    prompt(p);
    printf("Your amount of lifebelts is: %d\n", p->current_deck.lifebelts);
}
#endif

/* Wasserstand a und b ausgeben */
#if 0
void show_waterlevel(int a, int b, player *p)
{
    prompt(p);
    printf("The new \"watercards\" are %d and %d\n", a, b); 
}
#endif

/* Liest die auszuspielende Wassergarte ein */
#if 0
int get_weather(player *p)
{
    bool indeck = false;
    int i, c;
    while (!indeck) {
        prompt(p);
        printf("Please enter your chosen \"watercard\": ");
        c = getint();
        for (i = 0; i < 12; i++) {
            if (c > 0 && c == p->current_deck.weathercards[i])
                indeck = true;
        }
    }
    return c;
}
#endif

/* Gibt alle Wasserstände aus */
#if 0
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
#endif

/* Gibt alle Punkte aus */
#if 0
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
#endif

/* vim: set sw=4 ts=4 fdm=syntax: */
