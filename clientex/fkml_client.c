/* fkml_client.c - the real one...fkml_client */
#include "fkml_client.h"

#define BUF_SIZE    (1024)

int main(int argc, char *argv[])
{
    int sock_num;
    FILE *fpsock;
    int port;
    char *ip;
    if (argc != 3) {
	ip = "127.0.0.1";
	port = 1337;
    } else {
	ip = *(++argv);
	port = atoi(*++argv);
    }

    char *input;
    player *p = create_player();
    player **enemys = NULL;
    int enemys_count = 0;
    int i;

    /* creating interface */
    initialise_windows();
    write_win(CHAT_BOX, "Chat is disabled...\n");

    /* creating socket */
    sock_num = create_sock();
    fpsock = fdopen(sock_num, "a+");

    /* STARTING GAME */
    /* login */
    do {
	p->name = request_nick();
	connect_server(sock_num, ip, port);

	/* printing server's banner
	input = receive_from(fpsock);
	write_win(GAME_BOX, "Server sent: %s\n", input); */

	write_win(GAME_BOX, "Try to login with nick %s...", p->name);
	send_to(fpsock, "LOGIN %s\n", p->name);

	input = receive_from(fpsock);
    } while (strncmp(input, "ACK", 3) != 0);
    write_win(GAME_BOX, "Login succeed!\n");
    
    /* start */
    do {
	input = receive_from(fpsock);
    } while (strncmp(input, "START ", 6) != 0);
    parse_start(input, enemys, &enemys_count);

    write_win(GAME_BOX, "Game is starting with %d players:\n", enemys_count);
    for (i = 0; i < enemys_count; i++)
	write_win(GAME_BOX, "%s ", enemys[i]->name[i]); 


    /* deck */
    do {
	input = receive_from(fpsock);
    } while (strncmp(input, "DECK ", 5) != 0);
    parse_deck(p, input);
    print_deck(p);

    /* rings */
    do {
	input = receive_from(fpsock);
    } while (strncmp(input, "RINGS ", 6) != 0);

    /*
    if (strncmp(input, "MSGFROM ", 8) == 0)
	write_win(GAME_BOX, "%s", &input[8]); 
    if (DEBUG && strlen(input) > 0)
	write_win(GAME_BOX, "INPUT IS: %s", input);
    */


    getch();


    destroy_windows();
    close(sock_num);

    return 0;
}

