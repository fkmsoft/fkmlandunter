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
	//sock_num = create_sock();
	//connect_server(sock_num, ip, port);

	/* printing server's banner
	input = receive_from(fpsock);
	write_win(GAME_BOX, "Server sent: %s\n", input); */

	write_win(GAME_BOX, "Try to login with nick %s...", p->name);
	send_to(fpsock, "LOGIN %s\n", p->name);

	input = receive_from(fpsock);
    } while (strncmp(input, "ACK", 3) != 0);
    write_win(GAME_BOX, "Login succeed =)!\n");
    
    /* start */
    do {
	input = receive_from(fpsock);
    } while (strncmp(input, "START ", 6) != 0);
    write_win(GAME_BOX, "Game is starting now =)!\n");
    /* ORDENTLICHE START NACHRICHT EINBAUEN -.- */
    /* Syntax = START PLAYERANZ NAME1 NAME2 NAME3 */

    /* deck */
    do {
	input = receive_from(fpsock);
    } while (strncmp(input, "DECK ", 5) != 0);
    p->current_deck = parse_deck(input);
    print_deck(p);

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

