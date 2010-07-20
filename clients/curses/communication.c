/* communication.c - the thickheaded communication interface */
#include "communication.h"


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
void parse_start(gamestr *game, char *s)
{
    int i, j, maxt;

    sscanf(s, "START %d", &game->count);

    /* creating villains */
    player *v = malloc(sizeof(player) * game->count);
    for (i = 0; i < game->count; i++) {
        v[i].points = 0;
        v[i].water_level = 0;
        v[i].dead = false;
        v[i].name = NULL;
        for (j = 0; j < 12; j++)
            v[i].weathercards[j] = 0;
        v[i].lifebelts = 0;
        v[i].tabnum = 0;
    }

    /* skipping first two items */
    strtok(s, " ");
    strtok(NULL, " ");
    
    maxt = 0; char *p;
    for (i = 0; i < game->count; i++) {
        p = strtok(NULL, " ");
        v[i].name = malloc(strlen(p) + 1);
        strcpy(v[i].name, p);

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

    /* freeing last char from newline */
    v[game->count-1].name[strlen(v[game->count-1].name) - 1] = '\0';

    game->villain = v;
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
        for (i = 0; i < g->count; i++) 
            g->villain[i].lifebelts = atoi(strtok(NULL, " "));
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
        return 0;
    }
    return (-1);
}

/* create and return socket */
int create_sock()
{
    int sock;
    const int y = 1;
    sock = socket(PF_INET, SOCK_STREAM, 0);
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(int));
    return sock;
}

/* connect to socket sock - return -1 if attempt fails, -2 if host unknown */
int connect_socket(int sock, char *serv_addr, unsigned short port)
{
   struct sockaddr_in server;
   struct hostent *host_info;
   unsigned long addr;
   memset(&server, 0, sizeof (server));
   if ((addr = inet_addr( serv_addr )) != INADDR_NONE)
       memcpy((char *)&server.sin_addr, &addr, sizeof(addr));
   else {
       host_info = gethostbyname(serv_addr);
       if (NULL == host_info)
           return (-2);
       memcpy((char *)&server.sin_addr, host_info->h_addr_list[0], host_info->h_length);
   }
   server.sin_family = AF_INET;
   server.sin_port = htons( port );
   if (connect(sock, (struct sockaddr *)&server, sizeof( server)) < 0)
      return (-1);
   return 0;
}

/* receive BUF_SIZE chars from *fp */
char *receive_from(FILE *fp)
{
    char *buffer = calloc(BUF_SIZE, sizeof(char));
    
    if (fgets(buffer, BUF_SIZE, fp) == NULL) {
        perror("receive_from");
        return NULL;
    }

    return buffer;
}

char *receive_from2(FILE *fp)
{
    static char buffer[BUF_SIZE];
    static int pos = 0;
    int c;

    while (true) {
        c = getc(fp);
        if (c == EOF)
            return NULL;
        if (pos < BUF_SIZE)
            buffer[pos++] = c;
        if (pos >= BUF_SIZE || c == '\n') {
            char *rtn = calloc(strlen(buffer) + 1, sizeof(char));
            strncpy(rtn, buffer, strlen(buffer));
            pos = 0;
            memset(buffer, '\0', BUF_SIZE);
            return rtn;
        }
    }
}

/* send formated string *fmt to *fp */
void send_to(FILE *fp, char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vfprintf(fp, fmt, args);
    fflush(fp);

    va_end(args);
}

/* poll from fd inputA and inputB
 * return fd for POLLIN
 * return -1 for POLLRDHUP
 * return -2 if polling failed */
int poll_input(int sock, int inputB)
{
    struct pollfd pfds[2];

    pfds[0].fd = sock;
    pfds[0].events = POLLIN | POLLRDHUP;
    pfds[1].fd = inputB;
    pfds[1].events = POLLIN;

    if (poll(pfds, 2, -1) < 0)
        return -2;

    if (pfds[0].revents & POLLRDHUP)
        return -1;
    if (pfds[0].revents & POLLIN)
        return sock;
    if (pfds[1].revents & POLLIN)
        return inputB;

    return -2;
}

/* vim: set sw=4 ts=4 et fdm=syntax: */
