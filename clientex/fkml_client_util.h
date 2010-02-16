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
#include "interface.h"

int create_sock();

int connect_socket(int *sock, char *serv_addr, unsigned short port);

void connect_server(int socket, char *ip, int port);

char *request_nick();

char *receive_from(FILE *fp);

char *receive_from2(int fd);

void send_to(FILE *fp, char *fmt, ...);

void send_to2(int fd, char *format, ...);

int select_input(int inputA, int inputB);

void error_exit(char *error_message, int socket);

#endif
