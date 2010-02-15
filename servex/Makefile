CC = gcc
CFLAGS = -g -Wall
#CFLAGS = -g -Wall -ansi

HEADERS = fkml_server.h queue.h
EOBJS = echosrv.o fkml_server.o queue.o ../communication.o
OBJS = $(MOBJS) $(EOBJS)
TARGS = echosrv

all: $(TARGS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

echosrv: $(EOBJS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf $(OBJS) $(TARGS)
