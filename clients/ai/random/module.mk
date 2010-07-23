DIR := clients/ai/random

TARGETS += $(DIR)/random
randomobjs := $(DIR)/random.o $(DIR)/../../communication.o
randomhs := $(DIR)/../../communication.h
randomdeps := $(randomobjs) $(randomhs)
OBJS += $(randomobjs)

all:

$(DIR)/random: $(randomdeps)
	$(CC) $(CFLAGS) -o $@ $(randomobjs)
