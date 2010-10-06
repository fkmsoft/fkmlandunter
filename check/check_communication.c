#include "checks.h"
#include "../clients/communication.h"

START_TEST (test_communication_parse_start)
{
    int r;
    gamestr g;
    char s[100] = "START 3 One Two Three\n";

    g.player.name = "One";
    r = parse_start(&g, s);
    fail_unless(r == 0, "wrong player position");
    fail_unless(g.count == 3, "wrong number of players detected");
    fail_unless(g.villain != NULL, "no enemy array created");
    fail_unless(strcmp(g.villain[0].name, "One") == 0, "first name wrong");
    fail_unless(strcmp(g.villain[1].name, "Two") == 0, "first name wrong");
    fail_unless(strcmp(g.villain[2].name, "Three") == 0, "first name wrong");
}
END_TEST

START_TEST (test_communication_parse_deck)
{
    int i;
    gamestr g;
    char s[100] = "DECK 1 2 3 4 5 6 7 8 9 10 11 12\n";

    parse_cmd(&g, s);
    fail_unless(g.deck, "deck not set");
    for (i = 0; i < 12; i++)
        fail_unless(g.player.weathercards[i] == i + 1,
                "failed parsing card %d", i);
}
END_TEST

START_TEST (test_communication_parse_lifebelts)
{
    int i;
    gamestr g;
    char *s = strdup("RINGS 1 2 3 4 5\n");

    g.count = 5;
    fail_unless(g.count == 5, "player count not set");
    parse_cmd(&g, s);
    fail_unless(g.rings, "lifebelts not set");
    for (i = 0; i < 5; i++)
        fail_unless(g.villain[i].lifebelts == i + 1,
                "failed parsing lifebelt %d", i);
}
END_TEST
Suite *communication_suite (void)
{
    Suite *s;
    TCase *tc_core;
    
    s = suite_create("communication");

    /* Core test case */
    tc_core = tcase_create("core");

    tcase_add_test(tc_core, test_communication_parse_start);
    tcase_add_test(tc_core, test_communication_parse_deck);
    tcase_add_test(tc_core, test_communication_parse_lifebelts);

    suite_add_tcase (s, tc_core);

    return s;
}

/* vim: set sw=4 ts=4 et fdm=syntax: */
