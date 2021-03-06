dir := clients/sdl
targets += $(dir)/randall $(dir)/gui_test $(dir)/client

randallobjs := $(dir)/randall.o $(dir)/net_util.o $(dir)/gui_util.o $(dir)/../parse.o
randallhs   := $(dir)/net_util.h $(dir)/gui_util.h $(dir)/../parse.h
randalldeps := $(randallobjs) $(randallhs)

guitestobjs := $(dir)/gui_test.o $(dir)/gui_util.o $(dir)/text_util.o
guitesths   := $(dir)/gui_util.h $(dir)/text_util.h
guitestdeps := $(guitestobjs) $(guitesths)

clientobjs  := $(dir)/client.o $(dir)/net_util.o $(dir)/gui_util.o $(dir)/../parse.o \
		$(dir)/config.o $(dir)/text_util.o
clienths    := $(randallhs) $(dir)/config.h $(dir)/text_util.h
clientdeps := $(clientobjs) $(clienths)

sdl_flags   := `sdl2-config --cflags`
sdl_cmd     := `sdl2-config --libs`
sdl_net_cmd := -lSDL2_net
sdl_gui_cmd := -lSDL2_image -lSDL2_ttf
objs += $(randallobjs) $(guitestobjs) $(clientobjs)

CFLAGS += -std=c99

all:

$(dir)/randall: $(randalldeps)
	@echo "  CCLD" $@
	@$(LINK.c) $(sdl_flags) -o $@ $(randallobjs) $(sdl_cmd) $(sdl_net_cmd) $(sdl_gui_cmd)

$(dir)/gui_test: $(guitestdeps)
	@echo "  CCLD" $@
	@$(LINK.c) $(sdl_flags) -o $@ $(guitestobjs) $(sdl_cmd) $(sdl_gui_cmd)

$(dir)/client: $(clientdeps)
	@echo "  CCLD" $@
	@$(LINK.c) $(sdl_flags) -o $@ $(clientobjs) $(sdl_cmd) $(sdl_net_cmd) $(sdl_gui_cmd)
