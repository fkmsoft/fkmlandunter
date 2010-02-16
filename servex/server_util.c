/* server_util.c
 *
 * (c) Fkmsoft, 2010
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "server_util.h"
/* #include "fkml_server.h" */

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

    return arr;
}

/* create + prepare num players for the game */
void create_players(player *p_array, int num)
{
    int i;
    deck *decks = create_decks(num);

    for (i = 0; i < num; i++) {
        p_array[i].points = 0;
        p_array[i].water_level = 0;
        p_array[i].current_deck = decks[i];
        p_array[i].dead = false;
    }
}

/* creates an amount of num decks */
deck *create_decks(int num)
{
    deck *deck_arr = malloc(sizeof(deck)*num);
    int *cards = shuffle(60);
    int i, j, c;

    for (i = 0; i < num; i++) {
    for (j = 0; j < 12; j++) {
        c = deck_arr[i].weathercards[j] = cards[i*12 + j];
        if (c > 12 && c < 49)
        deck_arr[i].lifebelts += 1;
        if (c > 24 && c < 37)
        deck_arr[i].lifebelts += 2;
    }    
    deck_arr[i].lifebelts /= 2;
    }
    return deck_arr;
}

/* vim: set sw=4 ts=4 fdm=syntax: */
