CC = gcc
CFLAGS = -g -Wall #-ansi

ROBJS = random.o ../../fkml_client_util.o
RHEADS = ../../fkml_client_util.h
OBJS = $(ROBJS)
HEADERS = $(RHEADS)
TARGETS = random

all: $(TARGETS)

random: $(ROBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(OBJS) $(TARGETS)
