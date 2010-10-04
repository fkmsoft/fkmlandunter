/* communication.c - the thickheaded communication interface */
#include "parse.h"


/* Create and initialize game */
gamestr *create_game()
{
    int i;

    gamestr *game = malloc(sizeof(gamestr));

    /* count */
    game->count = 0;

    /* w_card[2] */
    game->w_card[0] = 0;
    game->w_card[1] = 1;

    /* round */
    game->round = 0;

    /* bools */
    game->deck = false;
    game->rings = false;
    game->weather = false;
    game->wlevel = false;
    game->points = false;

    /* message */
    game->message = false;
    game->msg_from = NULL;
    game->msg_data = NULL;

    /* creating player */
    game->player.points = 0;
    game->player.water_level = 0;
    game->player.dead = false;
    game->player.played = 0;
    game->player.name = malloc(MAXNICK * sizeof(char));
    for (i = 0; i < 12; i++)
        game->player.weathercards[i] = 0;
    game->player.lifebelts = 0;

    /* creating villain(s) - later in parse_start */
    game->villain = NULL;

    /* we don't know yet how may tabs we'll need */
    game->tabnum = 0;

    return game;
}

/* initialise gamestr->villains from string s */
int parse_start(gamestr *game, char *s)
{
    int i, j, maxt, pos;
    char *p;
    player *v;

    sscanf(s, "START %d", &game->count);

    /* creating villains */
    v = malloc(sizeof(player) * game->count);
    for (i = 0; i < game->count; i++) {
        v[i].points = 0;
        v[i].water_level = 0;
        v[i].dead = false;
        v[i].played = 0;
        v[i].name = NULL;
        for (j = 0; j < 12; j++)
            v[i].weathercards[j] = 0;
        v[i].lifebelts = 0;
        v[i].tabnum = 0;
    }

    /* skipping first two items */
    strtok(s, " ");
    strtok(NULL, " ");
    
    pos = -1;
    maxt = 0;
    for (i = 0; i < game->count; i++) {
        p = strtok(NULL, " ");

        /* freeing last name from newline */
        if (i == game->count-1) {
            *strchr(p, '\n') = '\0';
        }

        v[i].name = malloc(strlen(p) + 1);
        strcpy(v[i].name, p);

        if (strcmp(v[i].name, game->player.name) == 0)
            pos = i;

        v[i].tabnum = strlen(v[i].name) / TABSIZE;

        if (v[i].tabnum > maxt)
            maxt = v[i].tabnum;
    }

    for (i = 0; i < game->count; i++) {
        if (v[i].tabnum == maxt)
            v[i].tabnum = 1;
        else
            v[i].tabnum = 1 + maxt - v[i].tabnum;
    }
    game->tabnum = 1 + maxt;

    game->villain = v;

    return pos;
}

/* parse server command */
/* parse successful -> return 0
 * parse failed     -> return -1
 * string is msg    -> return 1 */
int parse_cmd(gamestr *g, char *s)
{
    int i;

    if (strncmp(s, "DECK ", 5) == 0) {
        for (i = 0; i < 12; i++)
            g->player.weathercards[i] = 0;
        sscanf(s, "DECK %d %d %d %d %d %d %d %d %d %d %d %d",
            &g->player.weathercards[0],  &g->player.weathercards[1],
            &g->player.weathercards[2],  &g->player.weathercards[3],
            &g->player.weathercards[4],  &g->player.weathercards[5],
            &g->player.weathercards[6],  &g->player.weathercards[7],
            &g->player.weathercards[8],  &g->player.weathercards[9],
            &g->player.weathercards[10], &g->player.weathercards[11]);
        g->deck = true; 
        return 0;
    } else if (strncmp(s, "RINGS ", 6) == 0) {
        strtok(s, " ");
        for (i = 0; i < g->count; i++) {
            g->villain[i].lifebelts = atoi(strtok(NULL, " "));

            if (g->villain[i].lifebelts == -1)
                g->villain[i].dead = true;
            else
                g->villain[i].dead = false;
        }
        g->rings = true;
        return 0;
    } else if (strncmp(s, "WEATHER ", 8) == 0) {
        strtok(s, " ");
        g->w_card[0] = atoi(strtok(NULL, " "));
        g->w_card[1] = atoi(strtok(NULL, " "));
        g->weather = true;
        return 0;
    } else if (strncmp(s, "WLEVELS ", 8) == 0) {
        strtok(s, " ");
        for (i = 0; i < g->count; i++)
            g->villain[i].water_level = atoi(strtok(NULL, " "));
        g->wlevel = true;
       return 0; 
    } else if (strncmp(s, "POINTS ", 7) == 0) {
        strtok(s, " ");
        for (i = 0; i < g->count; i++)
            g->villain[i].points = atoi(strtok(NULL, " "));
        g->round++;
        g->points = true;
        return 0;
    } else if (strncmp(s, "MSGFROM ", 8) == 0) {
        g->msg_from = strchr(s, ' ') + 1;
        g->msg_data = strchr(g->msg_from, ' ') + 1;

        *(g->msg_data - 1) = '\0';
        g->msg_data[strlen(g->msg_data) - 1] = '\0';
        
        g->message = true;
        return 1;
    } else if (strncmp(s, "PLAYED ", 7) == 0) {
        strtok(s, " ");
        for (i = 0; i < g->count; i++)
            g->villain[i].played = atoi(strtok(NULL, " "));
    }
    return (-1);
}

/* vim: set sw=4 ts=4 et fdm=syntax: */
