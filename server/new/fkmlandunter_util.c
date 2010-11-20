/* fkmlandunter_util.c
 *
 * Fkmlandunter utility functions
 *
 * (c) Fkmsoft, 2010
 */

#include "fkmlandunter_util.h"

/* returns int array with num numbers between 1 and num */
int *shuffle(int num)
{
    int i, r, *arr;

	if (num < 1)
		return 0;

	arr = calloc(sizeof(int), num);

    srand(time(NULL));

    for (i = 1; i <= num; i++) {
    do {
            r = rand()%num;
        } while (arr[r] != 0);
        arr[r] = i;
    }

    return arr;
}

/* creates an amount of num decks */
deck *create_decks(int num)
{
    int i, j, c, *cards;
    deck *deck_arr;

	if (num < 1 || num > 5)
		return 0;

	deck_arr = malloc(sizeof(deck)*num);
	cards = shuffle(60);

    for (i = 0; i < num; i++) {
		deck_arr[i].lifebelts = 0;
        for (j = 0; j < 12; j++) {
            c = deck_arr[i].weathercards[j] = cards[i*12 + j];
            if (c > 12 && c < 49)
                deck_arr[i].lifebelts += 1;
            if (c > 24 && c < 37)
                deck_arr[i].lifebelts += 1; /* we already got +1 above */
        }    
        deck_arr[i].lifebelts /= 2;
    }
    return deck_arr;
}

/* rotates an array of num decks by d steps */
deck *deck_rotate(deck *decks, int d, int num)
{
    deck *rotated;
    int i;

	if (!decks || num < 1)
		return 0;
	
	rotated = malloc(sizeof(deck)*num);
    for (i = 0; i < num; i++) {
        rotated[i] = decks[(num + i - d)%num];
    }

    return rotated;
}

player *create_player(int fd)
{
    player *p = calloc(1,sizeof(player));

    p->fd = fd;
	p->dead = true;

    return p;
}

void trim(char *str, char *evil)
{
    char *ep;
    for (ep = str + strlen(str); ep > str && strchr(evil, *(ep-1)); ep--)
        ;
    *ep = 0;
}

/* vim: set sw=4 ts=4 fdm=syntax: */
