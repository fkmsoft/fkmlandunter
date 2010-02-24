/* communication.c - the thickheaded communication interface */
#include "communication.h"


/* Create and initialize one player */
player *create_player()
{
    int i;

    /* creating player */
    player *p = malloc(sizeof(player));
    p->points = 0;
    p->water_level = 0;
    p->dead = false;
    p->name = ""; 
    for (i = 0; i < 12; i++)
        p->weathercards[i] = 0;
    p->lifebelts = 0;

    return p;
}

/* create opponents from string s */
opponents *parse_start(char *s)
{
    int i, j;

    opponents *oppo = malloc(sizeof(opponents));

    sscanf(s, "START %d", &oppo->count);

    /* creating player */
    player *p = malloc(sizeof(player) * oppo->count);
    for (i = 0; i < oppo->count; i++) {
        p[i].points = 0;
        p[i].water_level = 0;
        p[i].dead = false;
        p[i].name = NULL;
        for (j = 0; j < 12; j++)
            p[i].weathercards[j] = 0;
        p[i].lifebelts = 0;
    }

    /* skipping first two items */
    strtok(s, " ");
    strtok(NULL, " ");
    
    for (i = 0; i < oppo->count; i++)
        p[i].name = strtok(NULL, " ");
    /* freeing last char from newline */
    p[oppo->count-1].name[strlen(p[oppo->count-1].name) - 1] = '\0';

    oppo->p = p;
    return oppo;
}

/* parse deck from string s to player p */
void parse_deck(player *p, char *s)
{
    int i;
    for (i = 0; i < 12; i++)
        p->weathercards[i] = 0;
    sscanf(s, "DECK %d %d %d %d %d %d %d %d %d %d %d %d",
        &p->weathercards[0], &p->weathercards[1],  &p->weathercards[2],
        &p->weathercards[3], &p->weathercards[4],  &p->weathercards[5],
        &p->weathercards[6], &p->weathercards[7],  &p->weathercards[8],
        &p->weathercards[9], &p->weathercards[10], &p->weathercards[11]);
}

/* parse rings from string s to opponents o */
void parse_rings(opponents *o, char *s)
{
    int i;
    strtok(s, " ");
    for (i = 0; i < o->count; i++) 
        o->p[i].lifebelts = atoi(strtok(NULL, " "));
}

/* parse weathercards from string s to w_card */
void parse_weather(int *w_card, char *s)
{
    strtok(s, " ");
    w_card[0] = atoi(strtok(NULL, " "));
    w_card[1] = atoi(strtok(NULL, " "));
}

/* parse waterlevel from string s to opponents o */
void parse_wlevels(opponents *o, char *s)
{
    int i;
    strtok(s, " ");
    for (i = 0; i < o->count; i++)
        o->p[i].water_level = atoi(strtok(NULL, " "));
}

/* parse points from string s to opponents o */
void parse_points(opponents *o, char *s)
{
    int i;
    strtok(s, " ");
    for (i = 0; i < o->count; i++)
        o->p[i].points = atoi(strtok(NULL, " "));
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
    
    fgets(buffer, BUF_SIZE, fp);

    return buffer;
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

/* select from fd inputA and inputB */
int select_input(int inputA, int inputB)
{
    fd_set re;
    FD_ZERO(&re);
    FD_SET(inputA, &re);
    FD_SET(inputB, &re);

    select(max(inputA, inputB) + 1, &re, NULL, NULL, NULL);

    if (FD_ISSET(inputA, &re))
        return inputA;

    if (FD_ISSET(inputB, &re))
        return inputB;

    return (-1);
}

/*
char *receive_from2(int fd)
{
    char *buffer = calloc(BUF_SIZE, sizeof(char));

    if (read(fd, buffer, BUF_SIZE) < 0)
       //error_exit("Error receiving from %d", fd);

    return buffer;
}

void send_to2(int fd, char *format, ...)
{
    va_list ap;
    char *p, *sval;

    va_start(ap, format);
    for (p = format; *p; p++) {
        if (*p != '%') {
            if (send(fd, p, sizeof(*p), 0) == -1)
                error_exit("Error sending to %d", fd);
            continue;
        }
        switch (*++p) {
        case 's':
            for (sval = va_arg(ap, char *); *sval; sval++)
                if (send(fd, sval, sizeof(*sval), 0) == -1)
                    error_exit("Error sending to %d", fd);
            break;
        default:
            if (send(fd, p, sizeof(*p), 0) == -1)
                error_exit("Error sending to %d", fd);
            break;
        }
    }        
    va_end(ap);
}
*/

/* vim: set sw=4 ts=4 et fdm=syntax: */
