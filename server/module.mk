DIR := server
TARGETS += $(DIR)/server $(DIR)/echosrv
SERVEROBJS := $(DIR)/ncom.o $(DIR)/server.o $(DIR)/server_util.o \
	$(DIR)/fkml_server.o
ECHOOBJS := $(DIR)/echosrv.o $(DIR)/fkml_server.o $(DIR)/ncom.o
OBJS += $(SERVEROBJS) $(ECHOOBJS)

$(DIR)/server: $(SERVEROBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(DIR)/echosrv: $(ECHOOBJS)
	$(CC) $(CFLAGS) -o $@ $^
