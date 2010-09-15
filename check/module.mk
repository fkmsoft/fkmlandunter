dir := check
tests += $(dir)/check_main

checkmainobjs := $(dir)/check_main.o \
		$(dir)/check_fkmlist.o $(dir)/../server/new/fkmlist.o \
		$(dir)/check_communication.o $(dir)/../clients/communication.o

checkmainhs   := $(dir)/checks.h \
  		$(dir)/../server/new/fkmlist.h \
		$(dir)/../clients/communication.h

checkmaindeps := $(checkmainobjs) $(checkmainhs)
checkmainlibs := -lcheck

objs += $(checkmainobjs)

all:

$(dir)/check_main: $(checkmaindeps)
	@echo "  CCLD" $@
	@$(LINK.c) -o $@ $(checkmainobjs) $(checkmainlibs)
