CC = gcc
PFLAGS = -w -O
DFLAGS = -Wall -g

POBJS = fkml_client.o socketlayer.o
DOBJS = fkml_client.do socketlayer.do
HEADERS = fkml_client.h socketlayer.h
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
