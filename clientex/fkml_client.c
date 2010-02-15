/* fkml_client.c - the real one...fkml_client */
#include "fkml_client.h"

#define BUF_SIZE    (1024)
#define DEBUG	    (true)

int main(int argc, char *argv[])
{
    int sock_num;
    int port;
    char *ip;
    if (argc != 3) {
	ip = "127.0.0.1";
	port = 1337;
    } else {
	ip = *(++argv);
	port = atoi(*++argv);
    }

    /* creating interface */
    initialise_windows();
    getch();
    char te[] = "TOOONIIII!";
    int i;
    for (i = 0; i < 20; i++) {
	write_win(CHAT_BOX, "This is just a test!%s, no. %d\n", te, i);
	getch();
    }

    destroy_windows();

    /* creating socket */
    //sock_num = create_sock();

    /* connect to server */
    //connect_server(sock_num, ip, port);

    /* request nick */
    //char *nick = request_nick(); 

    /* print servers banner */
    //char *input = receive_from(sock_num);
    //print_gamewin("Server send: ");
    //print_gamewin(input);

    /* try to login */
    //login_server(sock_num, nick);

    
    /* WIIIIINNNNNDOOOOOWWWWWSSSSS */
    //write_win(2, "This is just a test my friend!");
    //getchar();
    
    //destroy_windows();


    /* STARTING GAME */
    /*
    do {
	input = receive_from(select_input(0, sock_num));

	if (strncmp(input, "MSGFROM ", 8) == 0)
	    printf("%s", &input[8]); 
	if (DEBUG && strlen(input) > 0)
	    printf("INPUT IS: %s", input);
    } while (true); //strncmp(buffer, "ACK", 3) != 0);

    printf("Nick acknowledged, proceeding game...\n");

    close_socket(sock_num);
    */
    return 0;
}

