dir := clients/sdl
targets += $(dir)/randall $(dir)/gui_test $(dir)/client

randallobjs := $(dir)/randall.o $(dir)/net_util.o $(dir)/gui_util.o $(dir)/../communication.o
randallhs   := $(dir)/net_util.h $(dir)/gui_util.h $(dir)/../communication.h
randalldeps := $(randallobjs) $(randallhs)

guitestobjs := $(dir)/gui_test.o $(dir)/gui_util.o $(dir)/text_util.o
guitesths   := $(dir)/gui_util.h $(dir)/text_util.h
guitestdeps := $(guitestobjs) $(guitesths)

clientobjs  := $(dir)/client.o $(dir)/net_util.o $(dir)/gui_util.o $(dir)/../communication.o \
		$(dir)/config.o
clienths    := $(randallhs) $(dir)/config.h
clientsdeps := $(clientobjs) $(clienths)

sdl_cmd     := `sdl-config --libs --cflags`
sdl_net_cmd := -lSDL_net
sdl_gui_cmd := -lSDL_image -lSDL_ttf
objs += $(randallobjs) $(guitestobjs) $(clientobjs)

all:

$(dir)/randall: $(randalldeps)
	$(LINK.c) $(sdl_cmd) $(sdl_net_cmd) $(sdl_gui_cmd) -o $@ $(randallobjs)

$(dir)/gui_test: $(guitestdeps)
	$(LINK.c) $(sdl_cmd) $(sdl_gui_cmd) -o $@ $(guitestobjs)

$(dir)/client: $(clientsdeps)
	$(LINK.c) $(sdl_cmd) $(sdl_net_cmd) $(sdl_gui_cmd) -o $@ $(clientobjs)
