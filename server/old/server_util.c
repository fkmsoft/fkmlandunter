/* server_util.c
 *
 * (c) Fkmsoft, 2010
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "server_util.h"
/* #include "fkml_server.h" */

/* not nice, but... */
#define debug false

/* returns int array with num numbers between 1 and num */
int *shuffle(int num)
{
    int i, r, *arr = calloc(sizeof(int), num);

    srand(time(NULL));

    for (i = 1; i <= num; i++) {
    do {
            r = rand()%num;
        } while (arr[r] != 0);
        arr[r] = i;
    }

    /* check it: */
	if (debug) {
		int *b = calloc(sizeof(int), num);
		for (i = 0; i < num; i++)
			b[arr[i] - 1]++;

		for (i = 0; i < num; i++)
			if (b[i] != 1)
				printf("FAIL in shuffle(%d) at %d!\n", num, i);

		free(b);
	}

    return arr;
}

/* create + prepare num players for the game */
void create_players(player *p_array/*, deck *d_array*/, int num)
{
    int i;
    /* deck *decks = create_decks(num); */

    for (i = 0; i < num; i++) {
        p_array[i].points = 0;
        p_array[i].water_level = 0;
        /* p_array[i].current_deck = d_array[i]; */
        p_array[i].dead = false;
        p_array[i].played = 0;
    }
}

/* creates an amount of num decks */
deck *create_decks(int num)
{
    deck *deck_arr = malloc(sizeof(deck)*num);
    int *cards = shuffle(60);
    int i, j, c;

    for (i = 0; i < num; i++) {
		deck_arr[i].lifebelts = 0;
        for (j = 0; j < 12; j++) {
            c = deck_arr[i].weathercards[j] = cards[12*i + j];
            if (c > 12 && c < 49)
                deck_arr[i].lifebelts += 1;
            if (c > 24 && c < 37)
                deck_arr[i].lifebelts += 1; /* we already got +1 above */
        }    
        deck_arr[i].lifebelts /= 2;

        if (debug && deck_arr[i].lifebelts > 12) {
            printf("LOTS of lifebelts in deck %d: %d\n", i, deck_arr[i].lifebelts);
            printf("deck %d:\n", i);
            for (j = 0; j < 12; j++)
                printf("%d, ", deck_arr[i].weathercards[j]);
            puts("");
        }
    }
    return deck_arr;
}

/* rotates an array of num decks by d steps */
deck *deck_rotate(deck *decks, int d, int num)
{
    deck *rotated = malloc(sizeof(deck)*num);
    int i;
    for (i = 0; i < num; i++) {
        rotated[i] = decks[(i + d)%num];
    }

    return rotated;
}

/* free the decks of all players in s */
void free_decks(fkml_server *s)
{
    int i;
    for (i = 0; i < s->connected; i++)
        free(&s->players[i].current_deck);
}

/* hand out num decks to num players */
void hand_decks(player *players, deck *decks, int num)
{
    int i;
    for (i = 0; i < num; i++)
        players[i].current_deck = decks[i];
}

/* vim: set sw=4 ts=4 et fdm=syntax: */
