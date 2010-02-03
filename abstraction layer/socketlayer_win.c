/* socketlayer.c – für MS-Windows */
#include <stdio.h>
#include <stdlib.h>
#include <winsock.h>
#include <io.h>
#define socket_t SOCKET
/* Funktion gibt aufgetretene Fehler aus und
 * beendet die Anwendung */
void error_exit(char *error_message) {
    fprintf(stderr,"%s: %d\n", error_message, WSAGetLastError());
    exit(EXIT_FAILURE);
}
/* Initialisiere TCP für Windows ("winsock"),
 * legt ein Socket an,
 * und gibt das Socket als Rückgabewert zurück */
int create_socket( int af, int type, int protocol ) {
    socket_t sock;
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD (1, 1);
    if (WSAStartup (wVersionRequested, &wsaData) != 0)
        error_exit( "Fehler beim Initialisieren von Winsock");
    else
        printf("Winsock initialisiert\n");
    /* Erzeuge das Socket */
    sock = socket(af, type, protocol);
    if (sock < 0)
        error_exit("Fehler beim Anlegen eines Socket");
    return sock;
}
/* Erzeugt die Bindung an die Serveradresse
 * (genauer an einen bestimmten Port) */
void bind_socket(socket_t *sock, unsigned long adress,
                 unsigned short port) {
   struct sockaddr_in server;
   memset( &server, 0, sizeof (server));
   server.sin_family = AF_INET;
   server.sin_addr.s_addr = htonl(adress);
   server.sin_port = htons(port);
   if (bind(*sock, (struct sockaddr*) &server,
          sizeof( server)) == SOCKET_ERROR)
       error_exit("Kann das Socket nicht \"binden\"");
}
/* Teile dem Socket mit, dass Verbindungswünsche
 * von Clients entgegengenommen werden */
void listen_socket( socket_t *sock ) {
  if(listen(*sock, 5) == –1 )
      error_exit("Fehler bei listen");
}
/* Bearbeite die Verbindungswünsche von Clients
 * Der Aufruf von accept() blockiert solange,
 * bis ein Client Verbindung aufnimmt */
void accept_socket( socket_t *socket, socket_t *new_socket ){
   struct sockaddr_in client;
   int len;
   len = sizeof(client);
   *new_socket=accept(*socket, (struct sockaddr *)&client, &len);
   if (*new_socket == INVALID_SOCKET)
      error_exit("Fehler bei accept");
}
/* Baut die Verbindung zum Server auf */
void connect_socket( socket_t *sock, char *serv_addr,
                     unsigned short port) {
   struct sockaddr_in server;
   struct hostent *host_info;
   unsigned long addr;
   memset( &server, 0, sizeof (server));
   if ((addr = inet_addr( serv_addr )) != INADDR_NONE) {
       /* argv[1] ist eine numerische IP-Adresse */
       memcpy( (char *)&server.sin_addr, &addr, sizeof(addr));
   }
   else {
       /* Für den Fall der Fälle: Wandle den
        * Servernamen bspw. "localhost" in eine IP-Adresse um */
       host_info = gethostbyname( serv_addr );
       if (NULL == host_info)
           error_exit("Unbekannter Server");
       memcpy( (char *)&server.sin_addr, host_info->h_addr,
                host_info->h_length);
   }
   server.sin_family = AF_INET;
   server.sin_port = htons( port );
   /* Baue die Verbindung zum Server auf */
   if (connect(
         *sock, (struct sockaddr*)&server, sizeof( server)) < 0)
      error_exit( "Kann keine Verbindung zum Server herstellen");
}
/* Daten versenden via TCP */
void TCP_send( socket_t *sock, char *data, size_t size ) {
   if( send (*sock, data, size, 0) == SOCKET_ERROR )
      error_exit("Fehler bei send()");
}
/* Daten empfangen via TCP */
void TCP_recv( socket_t *sock, char *data, size_t size) {
    int len;
    len = recv (*sock, data, size, 0);
    if( len > 0 || len != SOCKET_ERROR )
       data[len] = '\0';
    else
       error_exit("Fehler bei recv()");
}
/* Socket schließen und Winsock freigeben */
void close_socket( socket_t *sock ){
    closesocket(*sock);
}
void cleanup(void){
   /* Cleanup Winsock */
   WSACleanup();
   printf("Aufräumarbeiten erledigt ...\n");
}
