DIR := clients/curses
TARGETS += $(DIR)/client
CLIENTOBJS := $(DIR)/fkml_client.o $(DIR)/interface.o $(DIR)/communication.o
CLIENTHS := $(DIR)/fkml_client.h $(DIR)/interface.h $(DIR)/communication.h
CLIENTDEPS := $(CLIENTOBJS) $(CLIENTHS)
OBJS += $(CLIENTOBJS)
LIBS += -lncurses

$(DIR)/client: $(CLIENTDEPS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
