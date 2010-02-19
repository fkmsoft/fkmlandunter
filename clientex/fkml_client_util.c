/* fkml_client_util.c - utility lib for fkml_client */
#include "fkml_client_util.h"

#define BUF_SIZE    (1024)
#define max(A,B)    ((A) > (B) ? (A) : (B))

int create_sock()
{
    int sock;
    const int y = 1;
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        error_exit("Error creating socket", sock);
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(int));
    return sock;
}

/* returns (-1) if host unknown - returns (-2) if error connecting */
int connect_socket(int *sock, char *serv_addr, unsigned short port)
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
           return (-1);
       memcpy((char *)&server.sin_addr, host_info->h_addr, host_info->h_length);
   }
   server.sin_family = AF_INET;
   server.sin_port = htons( port );
   if (connect(*sock, (struct sockaddr *)&server, sizeof( server)) < 0)
      return (-2);
   return 0;
}

void connect_server(int socket, char *ip, int port)
{
    write_win(GAME_BOX, "IP = %s, Port = %d. Trying to connect...", ip, port);
    int connect = connect_socket(&socket, ip, port);
    if (connect ==  (0))
	write_win(GAME_BOX, "Connecting succeed\n");
    if (connect == (-1))
	error_exit("Host unknown", socket);
    if (connect == (-2))
	error_exit("Can't connect to server", socket); 
}

char *receive_from(FILE *fp)
{
    char *buffer = calloc(BUF_SIZE, sizeof(char));
    
    fgets(buffer, BUF_SIZE, fp);

    return buffer;
}

char *receive_from2(int fd)
{
    char *buffer = calloc(BUF_SIZE, sizeof(char));

    if (read(fd, buffer, BUF_SIZE) < 0)
       error_exit("Error receiving from %d", fd);

    return buffer;
}

void send_to(FILE *fp, char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vfprintf(fp, fmt, args);
    fflush(fp);

    va_end(args);
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

void error_exit(char *error_message, int socket)
{
    //getch();
    destroy_windows();
    fprintf(stderr, "%s: %s\n", error_message, strerror(errno));
    close(socket);
    exit(EXIT_FAILURE);
}

