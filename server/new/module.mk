DIR := server/new
TARGETS += $(DIR)/server

nserverobjs := $(DIR)/fkmserver.o $(DIR)/server.o $(DIR)/communication.o \
	$(DIR)/fkmlist.o $(DIR)/fkmlandunter.o $(DIR)/fkmlandunter_util.o
nserverhs := $(DIR)/fkmserver.h $(DIR)/fkmlist.h $(DIR)/communication.h \
	$(DIR)/fkmlandunter_util.h $(DIR)/fkmlandunter.h
nserverdeps := $(nserverobjs) $(nserverhs)
OBJS += $(nserverobjs)

all:

$(DIR)/server: $(nserverdeps)
	$(CC) $(CFLAGS) -o $@ $(nserverobjs)
