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
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> //inet_addr

#define BUFFERSIZ   100

int main(int argc, char *argv[])
{
    int sock_num; //good?
    int port;
    char *ip;
    if (argc != 3) {
	printf("Usage: %s PORT IP\n", argv[0]);
	printf("Example: %s 1337 127.0.0.1\n", argv[0]);
	return -1;
    } else {
	port = 0;
	++argv;
	while (isdigit(**(argv)))
	    port = 10 * port + *(*argv)++ - '0';
	ip = *(++argv);
    }

    printf("Chosen Port = %d\n", port);
    printf("Chosen IP   = %s\n", ip);;
   
    if ((sock_num = initialize(port, ip)) < 0) {
	printf("Connection failed\n");
	return -1;
    } else
	printf("Connection successful\n");

    /* STRARTING GAME */
    /* some variables... */
    printf("\nSTARTING FKMGAME (AARRRRRR):\n");
    int bytes;
    const char nick[] = "FlOREZ";
    char buffer[BUFFERSIZ];

    char *command;
    command = (char*)calloc(strlen("LOGIN ") + strlen(nick) + 1, sizeof(char));
    strcpy(command, "LOGIN ");
    strcat(command, nick);

    /* read the ******* banner dude! */
    if (bytes = read(sock_num, buffer, sizeof(buffer)) > 0)
	printf("Successfully recieved banner:\n%s", buffer);
    else
	printf("Error recieving server's banner\n");
    printf("Read %d bytes\n", bytes);
    
    if (bytes = write(sock_num, command, sizeof(*command)) > 0)
	printf("Successfully transmitted \"%s\"\n", command);
    else
	printf("Error transmitting \"%s\"\n", command);
    printf("Wrote %d bytes\n", bytes);
    
    /* XXX entweder echosrv antwortet nicht oder der folgende code ist kot */
    if (bytes = read(sock_num, buffer, sizeof(buffer)) > 0)
	printf("Successfully recieved answer:\n%s", buffer);
    else
	printf("Error recieving server's answer\n");
    printf("Read %d bytes\n", bytes);

    close(sock_num);
    return 0;
}

int initialize(int port, char *ip)
{
    /* check if input is valid */
    if (port < 0 || port > 65535) {
	perror("Error port is not valid");
	return -1;;

    }
    
    /* initialize connection */
    int sock_num;
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = inet_addr(ip);
    
    if ((sock_num = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
	perror("Error creating socket");
	return -1;
    }
    if (connect(sock_num, (struct sockaddr *)&address, sizeof(address)) < 0) {
	perror("Error connecting to host");
	close(sock_num);
	return -1;
    }

    return sock_num;
}
