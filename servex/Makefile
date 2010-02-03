CC = gcc
CFLAGS = -g -Wall -ansi

HEADERS = fkml_server.h queue.h
MOBJS = main.o fkml_server.o queue.o
EOBJS = echosrv.o fkml_server.o queue.o
OBJS = $(MOBJS) $(EOBJS)
TARGS = main echosrv

all: $(TARGS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

main: $(MOBJS)
	$(CC) $(CFLAGS) -o $@ $^

echosrv: $(EOBJS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf $(OBJS) $(TARGS)
