/* socketprx.h für MS-Windows */
#ifndef SOCKETPRX_H_
#define SOCKETPRX_H_
#include <stdio.h>
#include <stdlib.h>
#include <winsock.h>
#include <io.h>
#define socket_t SOCKET
void error_exit(char *error_message);
int create_socket( int af, int type, int protocol );
void bind_socket(socket_t *sock, unsigned long adress, 
  unsigned short port);
void listen_socket( socket_t *sock );
void accept_socket( socket_t *new_socket, socket_t *socket );
void connect_socket(socket_t *sock, char *serv_addr, 
  unsigned short port);
void TCP_send( socket_t *sock, char *data, size_t size);
void TCP_recv( socket_t *sock, char *data, size_t size);
void close_socket( socket_t *sock );
void cleanup(void);
#endif
