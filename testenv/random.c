/* random.c
 *
 * Randall!
 *
 * (c) Fkmsoft, 2010
 */

#define DEFPORT (1337)
#define DEFNAME ("Randall")
#define DEFHOST ("127.0.0.1")

/* for fdopen */
#define _POSIX_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../../communication.h"

int main(int argc, char **argv) {
    bool debug = false;
    char *name = DEFNAME;
   
    srand(time(NULL));
    int num = rand() % 10000;
    name = malloc((strlen(DEFNAME) + 6) * sizeof(char));
    sprintf(name, "%s%d", DEFNAME, num);

    int sock = create_sock();
    FILE *fp = fdopen(sock, "a+");
    if (connect_socket(sock, DEFHOST, DEFPORT) == -1) {
        fprintf(stderr, "%s did not find a server\n", name);
        exit(EXIT_FAILURE);
    }
    send_to(fp, "LOGIN %s\n", name);
    printf("%s verbunden\n", name);

    char *input, *p;
    int pos = 0, points = 0, i;

    /* position */
    do {
       input = receive_from(fp);
    } while (strncmp(input, "START ", 6) != 0);
    strtok(input, " "); 
    while (strcmp(strtok(NULL, " "), name) != 0)
        pos++;
                 
    while (1) {
        if ((input = receive_from(fp)) == 0)
            break;

        if (strncmp(input, "TERMINATE", 9) == 0)
            break;
        else if (strncmp(input, "DECK ", 5) == 0) {
            int card = 0;
            p = input + 5;
            while (!card)
                card = atoi(p++);
            printf("%s spielt %d\n", name, card);
            send_to(fp, "PLAY %d\n", card);
        } else if (strncmp(input, "POINTS ", 7) == 0) {
            strtok(input, " ");
            for (i = 0; i < pos; i++)
                points = atoi(strtok(NULL, " "));
            printf("%s.points = %d\n", name, points);
        } else if (debug)
            fputs(input, stdout);

        free (input);
    }

    fclose(fp);
    printf("%s hat fertig.\n", name);

    return points;
}

/* vim: set sw=4 ts=4 et fdm=syntax: */
