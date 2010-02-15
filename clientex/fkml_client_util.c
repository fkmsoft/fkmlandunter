/* fkml_client_util.c - utility lib for fkml_client */
#include "fkml_client_util.h"

#define	MAXNICK	    (30)
#define BUF_SIZE    (1024)
#define max(A,B)    ((A) > (B) ? (A) : (B))

int create_sock()
{
    int sock;
    const int y = 1;
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        error_exit("Error creating socket");
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
    write_win(1, "IP = %s, Port = %d. Trying to connect...\n", ip, port);
    int connect = connect_socket(&socket, ip, port);
    if (connect == (-1))
	error_exit("Host unknown");
    if (connect == (-2))
	error_exit("Can't connect to server"); 
}

char *request_nick()
{
    char *nick;
    nick = malloc(MAXNICK * sizeof(char));
    write_win(1, "Please enter your nickname: ");
    //fgets(nick, MAXNICK, stdin);
    nick = "Testeroni";
    return nick;
}

void login_server(int socket, char *nick)
{
    char *cmd;
    write_win(1, "Try to login with nick %s", nick);
    cmd = calloc(strlen("LOGIN ") + strlen(nick) + 1, sizeof(char));
    strcat(strcpy(cmd, "LOGIN "), strcat(nick, "\n"));
    TCP_send(&socket, cmd, strlen(cmd));
}

char *receive_from(int fd)
{
    char *buffer = calloc(BUF_SIZE, sizeof(char));
    if (read(fd, buffer, BUF_SIZE) < 0)
       error_exit("Fehler bei resv");
    return buffer;
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

void close_socket(int sock)
{
    close(sock);
}

void error_exit(char *error_message)
{
    fprintf(stderr, "%s: %s\n", error_message, strerror(errno));
    exit(EXIT_FAILURE);
}

//XXX UNBEDINGT ÄNDERN!!! schmuu aus alten zeiten...
/* Daten versenden via TCP */
void TCP_send(int *sock, char *data, size_t size) {
   if(send(*sock, data, size, 0) == -1)
      error_exit("Fehler bei send()");
}
