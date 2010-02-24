CC = gcc
CFLAGS = -g -Wall #-ansi

ROBJS = random.o ../../communication.o
RHEADS = ../../communication.h
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
