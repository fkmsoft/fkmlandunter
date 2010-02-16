CC = gcc
#CFLAGS = -g -Wall
CFLAGS = -g -Wall -ansi

HEADERS = fkml_server.h queue.h ncom.h
EOBJS = echosrv.o fkml_server.o queue.o ncom.o
TESTOBJS = server_util_test.o server_util.o
SERVEROBJS = ncom.o server.o server_util.o fkml_server.o queue.o
OBJS = $(MOBJS) $(EOBJS) $(TESTOBJS) $(SERVEROBJS)
TARGETS = echosrv server

all: $(TARGETS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

echosrv: $(EOBJS)
	$(CC) $(CFLAGS) -o $@ $^

server: $(SERVEROBJS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf $(OBJS) $(TARGETS)
