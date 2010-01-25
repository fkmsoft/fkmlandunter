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
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int sock_num; //not good...

int main()
{
    /* User Input */
    int port, i;
    int ip[4];
    printf("Port: "); port = getint();
    printf("IP: "); memset(ip, 0, sizeof(ip)); 
    for (i = 0; i < 4; i++)
	ip[i] = getint();

    if (initialize(port, ip) < 0) {
	printf("etwas schreckliches ist passiert\n");
	return -1;
    } else
	printf("Connected");

    /* read banner */
    int bytes;
    char buffer[BUFSIZ+1];
    
    while ((bytes = read(sock_num, buffer, 30)) > 0)
    printf("%d\n", bytes);
    printf("%s\n", buffer);
//	write(sock_num, buffer, bytes);

    close(sock_num);
    return 0;
}

int initialize(int port, int *ip)
{
    /* check if input is valid */
    int i;
    if (port < 0 || port > 65535)
	return -1;
    for (i = 0; i < 4; i++)
	if (ip[i] < 0 || ip[i] > 255)
	    return -2;

    /* initialize connection */
    int sock_num;
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = htonl((((((ip[0] << 8) | ip[1]) << 8) | ip[2]) << 8) | ip[3]);
    
    if ((sock_num = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
	perror("socket");
	return -3;
    }
    if (connect(sock_num, (struct sockaddr *)&address, sizeof(address)) < 0) {
	perror("connect");
	close(sock_num);
	return -4;
    }

    return 0;
}

/* reads from input till input is no int */
int getint()
{
    int c, n = 0;

    while (isdigit(c = getchar()))
	n = 10 * n + (c - '0');

    return n;
}
