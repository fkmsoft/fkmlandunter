dir := server/old

targets += $(dir)/server $(dir)/echosrv
serverobjs := $(dir)/ncom.o $(dir)/server.o $(dir)/server_util.o \
	$(dir)/fkml_server.o
echoobjs := $(dir)/echosrv.o $(dir)/fkml_server.o $(dir)/ncom.o
objs += $(serverobjs) $(echoobjs)

all:

$(dir)/server: $(serverobjs)
	$(LINK.c) -o $@ $^

$(dir)/echosrv: $(echoobjs)
	$(LINK.c) -o $@ $^
