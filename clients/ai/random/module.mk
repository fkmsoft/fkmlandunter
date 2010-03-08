DIR := clients/ai/random
TARGETS += $(DIR)/random
RANDOMOBJS := $(DIR)/random.o $(DIR)/../../curses/communication.o
RANDOMHS := $(DIR)/../../curses/communication.h
RANDOMDEPS := $(RANDOMOBJS) $(RANDOMHS)
OBJS += $(RANDOMOBJS)

$(DIR)/random: $(RANDOMDEPS)
	$(CC) $(CFLAGS) -o $@ $(RANDOMOBJS)
