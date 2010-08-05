#include "checks.h"

START_TEST (test_communication_simple)
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

Suite *communication_suite (void)
{
    Suite *s;
    TCase *tc_core;
    
    s = suite_create("communication");

    /* Core test case */
    tc_core = tcase_create("core");

    tcase_add_test(tc_core, test_communication_simple);

    suite_add_tcase (s, tc_core);

    return s;
}

/* vim: set sw=4 ts=4 et fdm=syntax: */
