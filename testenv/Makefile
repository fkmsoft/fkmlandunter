CC = gcc
CFLAGS = -g -Wall -ansi

ROBJS = random.o ../../communication.o
RHEADS = ../../communication.h
TOBJS = testenv.o
THEADS =
OBJS = $(ROBJS) $(TOBJS)
HEADERS = $(RHEADS) $(THEADS)
#TARGETS = random testenv
TARGETS = testenv

all: $(TARGETS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

random: $(ROBJS)
	$(CC) $(CFLAGS) -o $@ $^

testenv: $(TOBJS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf $(OBJS) $(TARGETS)
