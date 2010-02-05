/* socketprx.h für Linux/UNIX */
#ifndef SOCKETPRX_H_
#define SOCKETPRX_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

/* Funktionsprototypen */
void error_exit(char *error_message);

int create_socket(int af, int type, int protocol);

void bind_socket(int *sock, unsigned long adress, unsigned short port);

void listen_socket(int *sock);

void accept_socket(int *new_socket, int *socket);

void connect_socket(int *sock, char *serv_addr, unsigned short port);

void TCP_send(int *sock, char *data, size_t size);

void TCP_recv(int *sock, char *data, size_t size);

void close_socket(int *sock);

void cleanup(void);

#endif
