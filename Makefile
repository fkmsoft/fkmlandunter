CC = gcc
CFLAGS = -g -Wall
HEADERS = server_util.h communication.h

TESTOBJS = server_util_test.o server_util.o
SERVEROBJS = communication.o server.o server_util.o
OBJS = $(TESTOBJS) $(SERVEROBJS)
TARGETS = server server_util_test

all: server

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

server_util_test: $(TESTOBJS)
	$(CC) $(CFLAGS) -o $@ $^

server: $(SERVEROBJS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf $(OBJS) $(TARGETS)
