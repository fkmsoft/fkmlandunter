#ifndef FKML_CLIENT_UTIL_H
#define FKML_CLIENT_UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <netdb.h>

/* socket */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* select() */
#include <sys/select.h> // select()
#include <unistd.h>

/* manual */
#include "communication.h"

int create_sock();

int connect_socket(int *sock, char *serv_addr, unsigned short port);

void connect_server(int socket, char *ip, int port);

char *request_nick();

void login_server(int socket, char *nick);

char *receive_from(int fd);

int select_input(int inputA, int inputB);

void close_socket(int sock);

void error_exit(char *error_message);

void TCP_send(int *sock, char *data, size_t size); //XXX <--- schnetzeln!

#endif
