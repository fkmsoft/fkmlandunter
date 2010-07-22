DIR := server/old
TARGETS += $(DIR)/server $(DIR)/echosrv
serverobjs := $(DIR)/ncom.o $(DIR)/server.o $(DIR)/server_util.o \
	$(DIR)/fkml_server.o
echoobjs := $(DIR)/echosrv.o $(DIR)/fkml_server.o $(DIR)/ncom.o
OBJS += $(serverobjs) $(echoobjs)

all:

$(DIR)/server: $(serverobjs)
	$(CC) $(CFLAGS) -o $@ $^

$(DIR)/echosrv: $(echoobjs)
	$(CC) $(CFLAGS) -o $@ $^
