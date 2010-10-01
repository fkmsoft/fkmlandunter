/* config.c */

#include "config.h"

#define DEBUG_FILEPARSE 0

static char *usage = "Usage: %s [-d] [-n name] [-h host] [-p port] [-r 800x600] [-s font]\n" \
    "with:\n" \
    "\t-d\t\tenable debug information\n" \
    "\t-n name\t\tuse name to log into server\n" \
    "\t-h host\t\tuse host (may be hostname or IP) as server\n" \
    "\t-p port\t\tconnect to this port instead of the default\n" \
    "\t-r resolution\tuse this resolution instead of 800x600\n" \
    "\t-s font\t\tuse this font instead of the default (must be in ttf format)\n" \
    "\t-H\t\tprint this help message\n";

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
        /* fprintf(stderr, "Error opening config file `%s': %s\n",
                filename, strerror(errno)); */
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

            if ( (p = subhome(conf->datadir)) ) {
                free(conf->datadir);
                conf->datadir = p;
            }

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
        } else if HAVE("font", 4)
            n = strlen(p);
            conf->font = malloc(n);
            strncpy(conf->font, p, n - 1);

            if (DEBUG_FILEPARSE)
                printf("font: `%s'\n", conf->font);
        }
#undef HAVE
    }
}

void config_fromargv(int argc, char **argv, struct config_s *conf)
{
    int opt;

    while ((opt = getopt(argc, argv, "Hn:p:h:dr:fs:")) != -1) {
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
            printf(usage, argv[0]);
            exit(EXIT_SUCCESS);
        case 'f':
            /* this is handled in client.c */
            break;
        case 's':
            conf->font = optarg;
            break;
        default:
            printf(usage, argv[0]);
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
