CC = gcc
CFLAGS = -g -Wall -ansi
HEADERS = server_util.h communication.h

TESTOBJS = server_util_test.o server_util.o
SERVEROBJS = ncom.o server.o server_util.o servex/fkml_server.o servex/queue.o
OBJS = $(TESTOBJS) $(SERVEROBJS)
TARGETS = server

all: $(TARGETS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

server: $(SERVEROBJS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf $(OBJS) $(TARGETS)
