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
clientsdeps := $(clientobjs) $(clienths)

sdl_flags   := `sdl-config --cflags`
sdl_cmd     := `sdl-config --libs`
sdl_net_cmd := -lSDL_net
sdl_gui_cmd := -lSDL_image -lSDL_ttf
objs += $(randallobjs) $(guitestobjs) $(clientobjs)

all:

$(dir)/randall: $(randalldeps)
	@echo "  CCLD" $@
	@$(LINK.c) $(sdl_flags) -o $@ $(randallobjs) $(sdl_cmd) $(sdl_net_cmd) $(sdl_gui_cmd)

$(dir)/gui_test: $(guitestdeps)
	@echo "  CCLD" $@
	@$(LINK.c) $(sdl_flags) -o $@ $(guitestdeps) $(sdl_cmd) $(sdl_gui_cmd)

$(dir)/client: $(clientsdeps)
	@echo "  CCLD" $@
	@$(LINK.c) $(sdl_flags) -o $@ $(clientsdeps) $(sdl_cmd) $(sdl_net_cmd) $(sdl_gui_cmd)
