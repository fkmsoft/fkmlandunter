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
    int i;

    /* creating player */
    player *p = malloc(sizeof(player));
    p->points = 0;
    p->water_level = 0;
    p->dead = false;
    p->name = ""; 
    for (i = 0; i < 12; i++)
	p->weathercards[i] = 0;
    p->lifebelts = 0;

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
    int i;
    for (i = 0; i < 12; i++)
	p->weathercards[i] = 0;
    sscanf(s, "DECK %d %d %d %d %d %d %d %d %d %d %d %d",
	&p->weathercards[0], &p->weathercards[1],
        &p->weathercards[2], &p->weathercards[3],
        &p->weathercards[4], &p->weathercards[5],
        &p->weathercards[6], &p->weathercards[7],
	&p->weathercards[8], &p->weathercards[9],
	&p->weathercards[10], &p->weathercards[11]);
}

opponents *parse_start(char *s)
{
    int i, j;

    opponents *oppo = malloc(sizeof(opponents));

    sscanf(s, "START %d", &oppo->count);

    /* creating player */
    player *p = malloc(sizeof(player) * oppo->count);
    for (i = 0; i < oppo->count; i++) {
	p[i].points = 0;
	p[i].water_level = 0;
	p[i].dead = false;
	p[i].name = NULL;
	for (j = 0; j < 12; j++)
	    p[i].weathercards[j] = 0;
	p[i].lifebelts = 0;
    }

    /* skipping first two items */
    strtok(s, " ");
    strtok(NULL, " ");
    
    for (i = 0; i < oppo->count; i++)
	p[i].name = strtok(NULL, " ");
    /* freeing last char from newline */
    p[oppo->count-1].name[strlen(p[oppo->count-1].name) - 1] = '\0';

    oppo->opponent = p;
    return oppo;
}

void parse_rings(opponents *o, char *s) 
{
    int i;
    strtok(s, " ");
    for (i = 0; i < o->count; i++) 
	o->opponent[i].lifebelts = atoi(strtok(NULL, " "));
}


void parse_weather(int *w_card, char *s)
{
    strtok(s, " ");
    w_card[0] = atoi(strtok(NULL, " "));
    w_card[1] = atoi(strtok(NULL, " "));
}

void parse_wlevels(opponents *o, char *s)
{
    int i;
    strtok(s, " ");
    for (i = 0; i < o->count; i++)
	o->opponent[i].water_level = atoi(strtok(NULL, " "));
}

void parse_points(opponents *o, char *s)
{
    int i;
    strtok(s, " ");
    for (i = 0; i < o->count; i++)
	o->opponent[i].points = atoi(strtok(NULL, " "));
}

/* Hilfsmethode zum Einlesen einer Ganzzahl */
#if 0
static int getint()
{
    int c, n = 0;

    while((c = getchar()) != '\n')
        if(isdigit(c))
            n = 10 * n + (c - '0');

    return n;
}
#endif

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
