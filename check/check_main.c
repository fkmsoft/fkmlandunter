#include "checks.h"

int main(void)
{
    int failed;
    SRunner *sr;

    sr = srunner_create(0);
    srunner_add_suite(sr, fkmlist_suite());
    srunner_add_suite(sr, communication_suite());
    srunner_add_suite(sr, sdl_config_suite());

    srunner_run_all(sr, CK_NORMAL);
    failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return failed;
}

/* vim: set sw=4 ts=4 et fdm=syntax: */
