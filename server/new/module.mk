DIR := server/new
TARGETS += $(DIR)/server
NSERVEROBJS := $(DIR)/fkmserver.o $(DIR)/server.o $(DIR)/communication.o \
	$(DIR)/fkmlist.o $(DIR)/fkmlandunter.o $(DIR)/fkmlandunter_util.o
NSERVERHS := $(DIR)/fkmserver.h $(DIR)/fkmlist.h $(DIR)/communication.h \
	$(DIR)/fkmlandunter_util.h $(DIR)/fkmlandunter.h
OBJS += $(NSERVEROBJS) $(ECHOOBJS)

$(DIR)/server: $(NSERVEROBJS) $(NSERVERHS)
	$(CC) $(CFLAGS) -o $@ $(NSERVEROBJS)
