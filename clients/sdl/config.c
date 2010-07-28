/* config.c */

#include "config.h"

#define DEBUG_FILEPARSE 0

/* replace the first occurence of ~ with $HOME,
   return a reference to the result string, or NULL,
   if no replace happened */
static char *subhome(char *s);

void config_fromfile(char *filename, struct config_s *conf)
{
    char buf[BUFL], *p;
    int n;
    FILE *f;

    p = subhome(filename);

    if (p) {
        f = fopen(p, "r");
        free(p);
    } else {
        f = fopen(filename, "r");
    }

    if (!f) {
        fprintf(stderr, "Error opening config file `%s': %s\n",
                filename, strerror(errno));
        return;
    }

    while (fgets(buf, BUFL, f)) {
        p = buf;

        while (isspace(*p))
            p++;

        if (*p == '#')
            continue;

#define HAVE(a,b) (strncmp(a, p, b) == 0) { p += b; while (isspace(*p)) p++;
        if HAVE("datadir", 7)
            n = strlen(p);
            conf->datadir = malloc(n);
            strncpy(conf->datadir, p, n - 1);

            if (DEBUG_FILEPARSE)
                printf("datadir: `%s'\n", conf->datadir);
        } else if HAVE("resolution", 10)
            conf->x_res = atoi(p);
            p = strchr(p, 'x') + 1;
            conf->y_res = atoi(p);

            if (DEBUG_FILEPARSE)
                printf("resolution: %dx%d\n", conf->x_res, conf->y_res);
        } else if HAVE("name", 4)
            n = strlen(p);
            conf->name = malloc(n);
            strncpy(conf->name, p, n - 1);

            if (DEBUG_FILEPARSE)
                printf("name: `%s'\n", conf->name);
        } else if HAVE("host", 4)
            n = strlen(p);
            conf->host = malloc(n);
            strncpy(conf->host, p, n - 1);

            if (DEBUG_FILEPARSE)
                printf("host: `%s'\n", conf->host);
        } else if HAVE("port", 4)
            conf->port = atoi(p);

            if (DEBUG_FILEPARSE)
                printf("port: %d\n", conf->port);
        }
#undef HAVE
    }
}

void config_fromargv(int argc, char **argv, struct config_s *conf)
{
    int opt;

    while ((opt = getopt(argc, argv, "Hn:p:h:dr:f")) != -1) {
        switch (opt) {
        case 'n':
            conf->name = optarg;
            break;
        case 'p':
            conf->port = atoi(optarg);
            break;
        case 'h':
            conf->host = optarg;
            break;
        case 'd':
            conf->debug = true;
            break;
        case 'r':
            conf->x_res = atoi(optarg);
            conf->y_res = atoi(strchr(optarg, 'x') + 1);
            if (conf->debug)
                printf("have %dx%d\n", conf->x_res, conf->y_res);
            break;
        case 'H':
            printf("Usage: %s [-d] [-n name] [-h host] [-p port] [-r 800x600]\n", argv[0]);
            exit(EXIT_SUCCESS);
        case 'f':
            /* this is handled in client.c */
            break;
        default:
            printf("Usage: %s [-d] [-n name] [-h host] [-p port] [-r 800x600]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
}

static char *subhome(char *s)
{
    char *p, *q, *r, *result, *home;

    home = getenv("HOME");

    if ((p = strchr(s, '~'))) {
        result = malloc(strlen(s) + strlen(home) + 1);
        q = p;
        r = result;
        while (q != p)
            *r++ = *q++;

        while ((*r++ = *home++))
            ;

        r--;
        q++;

        while ((*r++ = *q++))
            ;
    } else {
        result = 0;
    }

    return result;
}

/* vim: set sw=4 ts=4 et fdm=syntax: */
