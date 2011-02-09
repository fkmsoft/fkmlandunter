dir := server/new
targets += $(dir)/server

CFLAGS += -DBLOAT

nserverobjs := $(dir)/fkmserver.o $(dir)/server.o $(dir)/communication.o \
	$(dir)/fkmlist.o $(dir)/fkmlandunter.o $(dir)/fkmlandunter_util.o
nserverhs := $(dir)/fkmserver.h $(dir)/fkmlist.h $(dir)/communication.h \
	$(dir)/fkmlandunter_util.h $(dir)/fkmlandunter.h

include $(dir)/bloat/module.mk
dir := server/new

nserverdeps := $(nserverobjs) $(nserverhs)
objs += $(nserverobjs)

all:

$(dir)/server: $(nserverdeps)
	@echo "  CCLD" $@
	@$(LINK.c) -o $@ $(nserverobjs)
