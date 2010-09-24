/* config.h */

#ifndef FKML_SDL_CONFIG_H
#define FKML_SDL_CONFIG_H

#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

struct config_s {
    char *datadir;
    char *name;
    char *host;
    char *font;
    int   port;
    int   x_res;
    int   y_res;
    bool  debug;
};

#define BUFL (1024)

void config_fromfile(char *filename, struct config_s *conf);
void config_fromargv(int argc, char **argv, struct config_s *conf);

/* example:

# comment
datadir     ~/code/landu/data
font        TOONISH.TTF
resolution  1280x960
name        Richard
host        127.0.0.1
port        1337

*/

#endif /* FKML_SDL_CONFIG_H */

/* vim: set sw=4 ts=4 et fdm=syntax: */
