CC = gcc
PFLAGS = -w -O -ansi -lncurses
DFLAGS = -Wall -g -ansi -lncurses

POBJS = fkml_client.o interface.o communication.o
DOBJS = fkml_client.do interface.do communication.do
HEADERS = fkml_client.h interface.h communication.h
OBJS = $(DOBJS) $(POBJS)

all: client client.db

%.o: %.c $(HEADERS)
	$(CC) $(PFLAGS) -c -o $@ $<

%.do: %.c $(HEADERS)
	$(CC) $(DFLAGS) -c -o $@ $<

client: $(POBJS)
	$(CC) $(PFLAGS) -o $@ $^

client.db: $(DOBJS)
	$(CC) $(DFLAGS) -o $@ $^

clean:
	rm -rf $(OBJS) client client.db
