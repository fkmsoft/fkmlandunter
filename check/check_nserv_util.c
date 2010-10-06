#include "checks.h"
#include "../server/new/fkmlandunter_util.h"

START_TEST (test_nserv_shuffle)
{
    int *cards, *ref, i, n;

    n = 10;

    fail_if((ref = calloc(n, sizeof(int))) == 0, "out of memory");
    fail_if((cards = shuffle(n)) == 0, "shuffle returned null pointer");
    
    for (i = 0; i < n; i++) {
        fail_if(cards[i] < 1, "card %d smaller than 1: %d", i, cards[i]);
        fail_if(cards[i] > n, "card %d bigger than %d: %d", i, n, cards[i]);
        ref[cards[i] - 1] = 1;
    }

    for (i = 0; i < n; i++)
        fail_unless(ref[i] == 1, "shuffled set did not contain %d", i);
}
END_TEST

START_TEST (test_nserv_shuffle_err)
{
    fail_unless(shuffle(-1) == 0, "shuffled -1 cards?");
    fail_unless(shuffle(0) == 0, "shuffled 0 cards?");
}
END_TEST

START_TEST (test_nserv_create_decks)
{
#if 0
typedef struct { /* deck */
    int weathercards[12];
    int lifebelts;
} deck;
#endif

    const int maxg = 60, /* global card maximum */
              maxd = 12; /* card max for one deck */
    int n, *ref, i, j, l, c;
    deck *d;

    n = 5;
    fail_if((ref = calloc(maxg, sizeof(int))) == 0, "out of memory");
    fail_if((d = create_decks(n)) == 0, "create_decks(%d) returned null pointer", n);

    for (i = 0; i < n; i++) {
        l = 0;
        for (j = 0; j < maxd; j++) {
            c = d[i].weathercards[j];

            /* fprintf(stderr, "l was %d, ", l); */
            if (c > maxg)
                fail("card with illegal number: %d", c);
            else if (c > 48)
                l = l; /* do nothing */
            else if (c > 36)
                l++;
            else if (c > 24)
                l += 2;
            else if (c > 12)
                l++;

            /* fprintf(stderr, "is %d (card %d)\n", l, c); */

            ref[c - 1] = 1;
        }

        l /= 2;
        fail_unless(d[i].lifebelts == l,
                "wrong number of lifebelts for deck %d: have %d, expected %d",
                i, d[i].lifebelts, l);
    }

    for (i = 0; i < maxg; i++)
        fail_unless(ref[i] == 1, "deck set did not contain %d", i);
}
END_TEST

START_TEST (test_nserv_create_decks_err)
{
    fail_unless(create_decks(-1) == 0, "created -1 deck?");
    fail_unless(create_decks(0) == 0, "created 0 decks?");
    fail_unless(create_decks(6) == 0, "created 6 decks?");
    fail_unless(create_decks(254) == 0, "created 254 decks?");

}
END_TEST

START_TEST (test_nserv_deckrotate)
{
    int i, j, k, n, step;
    deck *d, *e;

    for (n = 1; n < 6; n++) {
        for (k = 1; k < n + 1; k++) {
            d = create_decks(n);
            e = deck_rotate(d, k, n);

            for (i = 0; i < n; i++) {
                step = (i + k) % n;
                for (j = 0; j < 12; j++)
                    fail_unless(d[i].weathercards[j] == e[step].weathercards[j],
                            "deck %d changed weathercard %d: %d -> %d (rotation %d, step %d)",
                            i, j, d[i].weathercards[j], e[step].weathercards[j], k, step);

                fail_unless(d[i].lifebelts == e[step].lifebelts,
                        "deck %d changed number of lifebelts: %d -> %d (rotation %d, step %d)",
                        i, d[i].lifebelts, e[step].lifebelts, k, step);
            }
        }
    }
}
END_TEST

START_TEST (test_nserv_deckrotate_err)
{
    deck *d;

    d = create_decks(3);

    fail_unless(deck_rotate(d, 4, -1) == 0, "rotated -1 deck?");
    fail_unless(deck_rotate(d, 4, 0) == 0, "rotated 0 decks?");
    fail_unless(deck_rotate(0, 1, 1) == 0, "rotated NULL deck?");
}
END_TEST

START_TEST (test_nserv_create_player)
{
#if 0
typedef struct { /* player */
    int points;
    deck current_deck;
    int water_level;
    bool dead;
    int played;
    char *name;
    int fd;
} player;
#endif

    int i;
    player *p, q;

    q.points = q.water_level = q.played = 0;
    q.name = 0;
    q.dead = true;
    q.fd = 1;

    for (i = 0; i < 12; i++)
        q.current_deck.weathercards[i] = 0;
    q.current_deck.lifebelts = 0;

    fail_if((p = create_player(1)) == 0);

    fail_unless(p->points == q.points);
    fail_unless(p->water_level == q.water_level);
    fail_unless(p->played == q.played);
    fail_unless(p->current_deck.lifebelts == q.current_deck.lifebelts);
    fail_unless(p->name == q.name);
    fail_unless(p->dead == q.dead);
    fail_unless(p->fd == q.fd);

    for (i = 0; i < 12; i++)
        fail_unless(p->current_deck.weathercards[i] == q.current_deck.weathercards[i]);
}
END_TEST

Suite *nserv_util_suite (void)
{
    Suite *s;
    TCase *tc_core;
    
    s = suite_create("new server util funcs");

    /* Core test case */
    tc_core = tcase_create("core");

    tcase_add_test(tc_core, test_nserv_shuffle);
    tcase_add_test(tc_core, test_nserv_shuffle_err);
    tcase_add_test(tc_core, test_nserv_create_decks);
    tcase_add_test(tc_core, test_nserv_create_decks_err);
    tcase_add_test(tc_core, test_nserv_deckrotate);
    tcase_add_test(tc_core, test_nserv_deckrotate_err);
    tcase_add_test(tc_core, test_nserv_create_player);

    suite_add_tcase (s, tc_core);

    return s;
}

/* vim: set sw=4 ts=4 et fdm=syntax: */
