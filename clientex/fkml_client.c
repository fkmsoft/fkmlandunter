/* fkmsft
 *
 * fkmlandunter prtcl:
 * -----------------------------
 * -> LOGIN name
 * <- ACK nam
 * <- START 3 nam1 nam2 nam3
 *
 * <- WEATHER 7 8
 * <- DECK 3 1 2 56
 * -> PLAY 56
 * <- ACk 56
 * <- FAIL 56
 * <- WLEVELS 7 8 0
 * <- POINTS 1 2 -1
 *
 * -> LOGOUT bye
 * <- TERMINATE fuck off
 *
 * -> MSG fu all los0rZ
 * <- MSGFROM name fu all los0rZ
 * -----------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#include "fkml_client.h"

#define BSIZE	(512)
#define MAXNICK	(30)
#define DEBUG	(true)

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
	++argv;
	port = 0;
	while (isdigit(**(argv)))
	    port = 10 * port + *(*argv)++ - '0';
    }

    printf("IP = %s, Port = %d. Trying to connect...\n", ip, port);

    sock_num = create_socket(PF_INET, SOCK_STREAM, 0);
    if (sock_num < 0)
	printf("Error creating socket\n");

    connect_socket(&sock_num, ip, port);

    /* STARTING GAME */
    char *nick;
    char *cmd;
    char buffer[BSIZE];
    /* printing server's banner */
    TCP_recv(&sock_num, buffer, sizeof(buffer));
    printf("Server sent:\n%s", buffer);


    /* try to login */
    nick = malloc(MAXNICK * sizeof(char));
    printf("Please enter your nickname: ");
    scanf("%s", nick);
    printf("Try to login with nick %s...\n", nick);
    cmd = malloc(sizeof(char) * (strlen("LOGIN ") + strlen(nick) + 1));
    strcat(strcpy(cmd, "LOGIN "), strcat(nick, "\n"));

    TCP_send(&sock_num, cmd, strlen(cmd));
    do {
	TCP_recv(&sock_num, buffer, BSIZE);
	if (strncmp(buffer, "MSGFROM ", 8) == 0)
	    printf("%s", &buffer[8]); 
	if (DEBUG)
	    printf("DEBUG ON: SVR SENT: %s", buffer);
    } while (strncmp(buffer, "ACK", 3) != 0);
    printf("Nick acknowledged, proceeding game...\n");

    /*
    char input[] = "Hello little fkml sever!\n";
    do {
	printf("Sending: \n%s\n", input);
	TCP_send(&sock_num, input, strlen(input));
	TCP_recv(&sock_num, buffer, BSIZE);
	printf("The server answered: \n%s", buffer);
	//printf("Enter a new message for the server: ");
    } while (scanf("\nEnter a new message for the server %s\n", input) > 0);
    */

    close_socket(&sock_num);
    cleanup;

    return 0;
}

