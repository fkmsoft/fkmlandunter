#ifndef FKML_CLIENT
#define FKML_CLIENT

int main();

/* initializes tcp connection - returns socket */
int initialize(int port, char *ip);

#endif
