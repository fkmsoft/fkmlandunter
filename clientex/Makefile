CC = gcc
PFLAGS = -w -O
DFLAGS = -Wall -g -DDEBUG
HEADERS = fkml_client.h

POBJS = fkml_client.o
DOBJS = fkml_client.do
OBJS = $(DOBJS) $(POBJS)

.SUFFIXES:
.SUFFIXES: .c .o .do
.c.d:; $(CC) -c $@ $(PFLAGS) $<
.c.do:; $(CC) -c $@ $(DFLAGS) $<

client: $(POBJS)
	$(CC) $(PFLAGS) -o $@ $^

client.db: $(DOBJS)
	$(CC) $(DFLAGS) -o $@ $^

clean:
	rm -rf $(OBJS) client client.db
