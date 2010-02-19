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
    opponents *oppos = NULL;
    int i;
    int w_card[] = { 0, 0 };
    bool indeck;
    int wcard;
    int round = 0;

    /* creating interface */
    initialise_windows();
    write_win(CHAT_BOX, "Chat is disabled...\n");

    /* creating socket */
    sock_num = create_sock();
    fpsock = fdopen(sock_num, "a+");

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
    write_win(GAME_BOX, "Login succeed\n");
    
    /* start */
    do {
	input = receive_from(fpsock);
    } while (strncmp(input, "START ", 6) != 0);
    oppos = parse_start(input);

    write_win(GAME_BOX, "Game is starting with %d players\n", oppos->count);


    /* STARTING GAME */
    while (round < oppos->count) {
	/* deck */
	while (strncmp(input, "DECK ", 5) != 0) {
	    write_win(GAME_BOX, "A new round has been started...\n");
	    input = receive_from(fpsock);
	}
	parse_deck(p, input);
	write_win(GAME_BOX, "Your current weathercards are:\n");
	for (i = 0; i < 12; i++) {
	    if (p->weathercards[i] > 0) {
		write_win(GAME_BOX, "%d, ", p->weathercards[i]);
	    }
	}
	write_win(GAME_BOX, "\n");


	/* rings */
	while (strncmp(input, "RINGS ", 6) != 0)
	    input = receive_from(fpsock);
	parse_rings(oppos, input);
	write_win(GAME_BOX, "Lifebelt ranking:\n");
	for (i = 0; i < oppos->count; i++) {
	    if (oppos->opponent[i].lifebelts == (-1))
		write_win(GAME_BOX, "%s: drowned\n");
	    else
		write_win(GAME_BOX, "%s: %d\n", oppos->opponent[i].name, oppos->opponent[i].lifebelts);
	}

	/* weather */
	while (strncmp(input, "WEATHER ", 8) != 0)
	    input = receive_from(fpsock);
	parse_weather(w_card, input);
	write_win(GAME_BOX, "The new watercards are %d and %d\n", w_card[0], w_card[1]); 


	/* play */
	indeck = false;
	write_win(GAME_BOX, "Please choose a weathercard from your deck: ");
	while (!indeck) { 
	    read_win(GAME_BOX, input, 2);
	    wcard = atoi(input);
	    for (i = 0; i < 12; i++)
		if (wcard > 0 && wcard == p->weathercards[i])
		    indeck = true;
	}
	write_win(GAME_BOX, "%d\n", wcard);
	write_win(GAME_BOX, "Please wait till the other players acted\n");
	do {
	    send_to(fpsock, "PLAY %d\n", wcard);
	    input = receive_from(fpsock);
	} while (strncmp(input, "ACK ", 4) != 0);


	/* wlevels */
	while (strncmp(input, "WLEVELS ", 8) != 0)
	    input = receive_from(fpsock);
	parse_wlevels(oppos, input);
	write_win(GAME_BOX, "The waterlevels are:\n");
	for (i = 0; i < oppos->count; i++) {
	    if (oppos->opponent[i].water_level == (-1))
		write_win(GAME_BOX, "%s: drowned\n", oppos->opponent[i].name);
	    else
		write_win(GAME_BOX, "%s: %d\n", oppos->opponent[i].name, oppos->opponent[i].water_level);
	}

	/* points or next round? */
	input = receive_from(fpsock);
	if (strncmp(input, "POINTS ", 7) == 0) {
	    round++;
	    parse_points(oppos, input);
	    write_win(GAME_BOX, "The score is:\n");
	    for (i = 0; i < oppos->count; i++)
		write_win(GAME_BOX, "%s: %d\n", oppos->opponent[i].name, oppos->opponent[i].points);
	}
    }

    write_win(GAME_BOX, "END OF GAME!\n");
    getch();


    destroy_windows();
    close(sock_num);

    return 0;
}

