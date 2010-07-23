dir := clients/curses

targets += $(dir)/client
clientobjs := $(dir)/fkml_client.o $(dir)/interface.o $(dir)/../communication.o
clienths := $(dir)/fkml_client.h $(dir)/interface.h $(dir)/../communication.h
clientdeps := $(clientobjs) $(clienths)

objs += $(clientobjs)
curslib := -lncurses

all:

$(dir)/client: $(clientdeps)
	$(LINK.c) -o $@ $^ $(curslib)
