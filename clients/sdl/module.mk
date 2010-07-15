DIR := clients/sdl
TARGETS += $(DIR)/randall $(DIR)/gui_test

randallobjs := $(DIR)/randall.o $(DIR)/net_util.o
randallhs := $(DIR)/net_util.h
randalldeps := $(randallobjs) $(randallhs)

guitestobjs := $(DIR)/gui_test.o $(DIR)/gui_util.o
guitesths := $(DIR)/gui_util.h
guitestdeps := $(guitestobjs) $(guitesths)

sdl_net_cmd := `sdl-config --libs --cflags` -lSDL_net
sdl_gui_cmd := `sdl-config --libs --cflags` -lSDL_image -lSDL_ttf
OBJS += $(randallobjs) $(guitestobjs)

all:

$(DIR)/randall: $(randalldeps)
	$(CC) $(CFLAGS) $(sdl_net_cmd) -o $@ $(randallobjs)

$(DIR)/gui_test: $(guitestdeps)
	$(CC) $(CFLAGS) $(sdl_gui_cmd) -o $@ $(guitestobjs)
