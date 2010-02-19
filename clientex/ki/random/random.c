/* random.c
 *
 * Randall!
 *
 * (c) Fkmsoft, 2010
 */

#define DEFPORT (1337)
#define DEFNAME ("Randall")
#define DEFHOST ("127.0.0.1")

#include <stdio.h>
/* for getopt */
#include <unistd.h>

#include "../../fkml_client_util.h"

void write_win(int struct_no, char *format, ...)
{
    return;
}

void destroy_windows()
{
    return;
}

int main(int argc, char **argv) {
    char *name = DEFNAME, *host = DEFHOST;
    int opt, port = DEFPORT;

    while ((opt = getopt(argc, argv, "n:p:h:")) != -1) {
        switch(opt) {
            case 'n':
                name = optarg;
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case 'h':
                host = optarg;
                break;
            default:
                printf("Usage: %s [-n name] [-h host] [-p port]\n", name);
                exit(EXIT_FAILURE);
        }
    }

    int sock = create_sock();
    FILE *fp = fdopen(sock, "a+");
	connect_server(sock, host, port);
	send_to(fp, "LOGIN %s\n", name);

    char *input;
    bool play = true;

    while (play) {
        if ((input = receive_from(fp)) == 0) {
            play = false;
            break;
        }

        if (strncmp(input, "TERMINATE", 9) == 0)
            play = false;
        else if (strncmp(input, "DECK ", 5) == 0) {
            int card = atoi(input + 5);
            printf("%s play %d\n", name, card);
            send_to(fp, "PLAY %d\n", card);
        }
    }

    fclose(fp);
    printf("%s hat fertig.\n", name);

    return EXIT_SUCCESS;
}
