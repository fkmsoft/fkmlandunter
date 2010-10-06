#include "checks.h"
#include "../server/new/fkmlist.h"

START_TEST (test_fkmlist_simple)
{
    fkmlist *l;
    int *p, data;

    data = 5;
    l = fkmlist_create();

    l = fkmlist_append(l, &data);
    fail_unless(data == 5, "data modified upon insert");

    p = fkmlist_get(l, 0);
    fail_unless(p == &data, "get returns garbage");
    fail_unless(*p == 5, "data modified after insert");

    l = fkmlist_rm(l, 0, false);

    p = fkmlist_get(l, 0);
    fail_unless(p == NULL, "data not deleted");

    fkmlist_del(l, false);
}
END_TEST

START_TEST (test_fkmlist_long)
{
#define INSERTS 1000
    fkmlist *l;
    int *p, i, data[INSERTS];

    l = fkmlist_create();

    for (i = 0; i < INSERTS; i++) {
        data[i] = i;
        l = fkmlist_append(l, &data[i]);
        fail_unless(data[i] == i, "data modified upon insert");
    }

    for (i = 0; i < INSERTS; i++) {
        p = fkmlist_get(l, i);
        fail_unless(p == &data[i], "get returns garbage");
        fail_unless(*p == i, "data modified after insert");
    }

    for (i = 0; i < INSERTS; i++) {
        l = fkmlist_rm(l, 0, false);

        p = fkmlist_get(l, INSERTS - 1 - i);
        fail_unless(p == NULL, "data not deleted");

        if (i != INSERTS - 1) {
            p = fkmlist_get(l, INSERTS - 2 - i);
            fail_unless(p != NULL, "got garbage");
            fail_unless(*p == INSERTS - 1, "unrelated data deleted");
        }
    }

    fkmlist_del(l, false);
}
END_TEST

START_TEST (test_fkmlist_illegal)
{
    fkmlist *l;
    int *p, data;

    l = 0;
    p = fkmlist_get(l, 0);
    fail_unless(p == NULL, "random output (no list)");

    l = fkmlist_create();
    p = fkmlist_get(l, 0);
    fail_unless(p == NULL, "random output (empty list)");

    l = fkmlist_append(l, &data);
    p = fkmlist_get(l, 1);
    fail_unless(p == NULL, "accessed after end of list");

    p = fkmlist_get(l, 0);
    fail_unless(p == &data, "element lost");

    p = fkmlist_get(l, -1);
    fail_unless(p == NULL, "negative element accessed");

    fkmlist_del(l, false);

    /*
    l = fkmlist_append(l, &data);
    fail_unless(l == NULL, "appended after list deletion");
    */

    p = fkmlist_get(l, 0);
    fail_unless(p == NULL, "element accessed after list deletion");
}
END_TEST

Suite *fkmlist_suite (void)
{
    Suite *s;
    TCase *tc_core;
    
    s = suite_create("fkmlist");

    /* Core test case */
    tc_core = tcase_create("core");

    tcase_add_test(tc_core, test_fkmlist_simple);
    tcase_add_test(tc_core, test_fkmlist_long);
    tcase_add_test(tc_core, test_fkmlist_illegal);

    suite_add_tcase (s, tc_core);

    return s;
}

/* vim: set sw=4 ts=4 et fdm=syntax: */
