/* socketlayer.c – für Linux/UNIX */
#include  "socketlayer.h"

/* Funktion gibt aufgetretene Fehler aus und beendet die Anwendung */
void error_exit(char *error_message)
{
    fprintf(stderr, "%s: %s\n", error_message, strerror(errno));
    exit(EXIT_FAILURE);
}

int create_socket(int af, int type, int protocol)
{
    int sock;
    const int y = 1;
    /* Erzeuge das Socket */
    sock = socket(af, type, protocol);
    /* Mehr dazu siehe Anmerkung am Ende des Listings ... */
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(int));
    return sock;
}

/* Erzeugt die Bindung an die Serveradresse (genauer an einen bestimmten Port) */
void bind_socket(int *sock, unsigned long adress, unsigned short port)
{
   struct sockaddr_in server;
   memset(&server, 0, sizeof (server));
   server.sin_family = AF_INET;
   server.sin_addr.s_addr = htonl(adress);
   server.sin_port = htons(port);
   if (bind(*sock, (struct sockaddr*)&server, sizeof(server)) < 0)
       error_exit("Kann das Socket nicht \"binden\"");
}

/* Teile dem Socket mit, dass Verbindungswünsche von Clients entgegengenommen werden */
void listen_socket(int *sock)
{
  if (listen(*sock, 5) == -1)
      error_exit("Fehler bei listen");
}

/* Bearbeite die Verbindungswünsche von Clients
 * Der Aufruf von accept() blockiert solange,
 * bis ein Client Verbindung aufnimmt */
void accept_socket(int *socket, int *new_socket)
{
    struct sockaddr_in client;
    unsigned int len;
    len = sizeof(client);
    *new_socket = accept(*socket, (struct sockaddr *)&client, &len);
    if (*new_socket  == -1)
      error_exit("Fehler bei accept");
}

/* Baut die Verbindung zum Server auf */
/* returns (-1) if host unknown - returns (-2) if error connecting */
int connect_socket(int *sock, char *serv_addr, unsigned short port)
{
   struct sockaddr_in server;
   struct hostent *host_info;
   unsigned long addr;
   memset(&server, 0, sizeof (server));
   if ((addr = inet_addr( serv_addr )) != INADDR_NONE)
       /* argv[1] ist eine numerische IP-Adresse */
       memcpy((char *)&server.sin_addr, &addr, sizeof(addr));
   else {
       /* Wandle den Servernamen bspw. "localhost" in eine IP-Adresse um */
       host_info = gethostbyname(serv_addr);
       if (NULL == host_info)
           return (-1);
       memcpy((char *)&server.sin_addr, host_info->h_addr, host_info->h_length);
   }
   server.sin_family = AF_INET;
   server.sin_port = htons( port );
   /* Baue die Verbindung zum Server auf */
   if (connect(*sock, (struct sockaddr *)&server, sizeof( server)) < 0)
      return (-2);
   return 0;
}

/* Daten versenden via TCP */
void TCP_send(int *sock, char *data, size_t size) {
   if(send(*sock, data, size, 0) == -1)
      error_exit("Fehler bei send()");
}

/* Daten empfangen via TCP */
/* returns (-1) if recieving failed */
int TCP_recv(int *sock, char *data, size_t size) {
    int len;
    len = recv(*sock, data, size, 0);
    if(len > 0 || len != -1)
       data[len] = '\0';
    else
	return (-1);
    return 0;
}

/* Socket schließen */
void close_socket( int *sock ){
    close(*sock);
}

/* Unter Linux/UNIX nichts zu tun ... */
void cleanup(void){
   printf("Aufraeumarbeiten erledigt ...\n");
   return;
}

