DIR := clients/curses

TARGETS += $(DIR)/client
clientobjs := $(DIR)/fkml_client.o $(DIR)/interface.o $(DIR)/../communication.o
clienths := $(DIR)/fkml_client.h $(DIR)/interface.h $(DIR)/../communication.h
clientdeps := $(clientobjs) $(clienths)

OBJS += $(clientobjs)
curslib := -lncurses

all:

$(DIR)/client: $(clientdeps)
	$(CC) $(CFLAGS) -o $@ $^ $(curslib)
