#include "cmd.h"

conserver *setup(fkmserver *s, char *motd)
{
    conserver *c = conserver_init(s, motd);
    conserver_reg_user(c, "denis", "kopf");
    conserver_reg_cmd(c, "echo", echo);

    return c;
}

char *echo(fkmserver *slv, int argc, char **argv)
{
    char *s, **p, *q;
    int i, l;

    for (i = 1, l = 2, p = argv, p++; i < argc; i++, p++) {
        l += strlen(*p);
    }

    if (l != 2) {
        s = malloc(sizeof(char) * l);

        for (i = 1, q = s, p = argv, p++; i < argc; i++, p++) {
            q += sprintf(q, "%s ", *p);
        }

        sprintf(q, "\n");
    } else {
        s = 0;
    }

    return s;
}

/* vim: set sw=4 ts=4 et fdm=syntax: */
