#include "checks.h"
#include "../clients/sdl/config.h"

START_TEST (test_config_file_invalid)
{
    struct config_s c;
    char *invalidpath = "/dev/denis/config";

    fail_unless(config_fromfile(invalidpath, &c) == -1, "config_fromfile didn't indicate error");
}
END_TEST

START_TEST (test_config_exampleconfig)
{
    struct config_s c, ex;
    char *examplepath = "clients/sdl/example.fkmlandunterrc";

    ex.datadir = "/usr/share/fkmlandunter/data/";
    ex.font = "TOONISH.ttf";
    ex.name = "Nickname";
    ex.host = "127.0.0.1";

    ex.x_res = 1280;
    ex.y_res =  960;
    ex.port  = 1337;

    fail_unless(config_fromfile(examplepath, &c) == 0,
            "config_fromfile returned error");

    /* strings */
    fail_if(strcmp(c.datadir, ex.datadir),
            "datadir wrong, have `%s', expected `%s'",
            c.datadir, ex.datadir);

    fail_if(strcmp(c.font, ex.font),
            "font wrong, have `%s', expected `%s'",
            c.font, ex.font);

    fail_if(strcmp(c.name, ex.name),
            "nickname wrong, have `%s', expected `%s'",
            c.name, ex.name);

    fail_if(strcmp(c.host, ex.host),
            "host wrong, have `%s', expected `%s'",
            c.host, ex.host);


    /* integers */
    fail_unless(c.x_res == ex.x_res,
            "resolution x wrong, have %d expected %d",
            c.x_res, ex.x_res);

    fail_unless(c.y_res == ex.y_res,
            "resolution y wrong, have %d expected %d",
            c.y_res, ex.y_res);

    fail_unless(c.port == ex.port,
            "port wrong, have %d expected %d",
            c.port, ex.port);
}
END_TEST

Suite *sdl_config_suite (void)
{
    Suite *s;
    TCase *tc_core;
    
    s = suite_create("sdl config");

    /* Core test case */
    tc_core = tcase_create("core");

    tcase_add_test(tc_core, test_config_file_invalid);
    tcase_add_test(tc_core, test_config_exampleconfig);

    suite_add_tcase (s, tc_core);

    return s;
}

/* vim: set sw=4 ts=4 et fdm=syntax: */
