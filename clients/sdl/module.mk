DIR := clients/sdl
TARGETS += $(DIR)/randall $(DIR)/gui_test

RANDALLOBJS := $(DIR)/randall.o $(DIR)/sdl_util.o
RANDALLHS := $(DIR)/sdl_util.h
RANDALLDEPS := $(RANDALLOBJS) $(RANDALLHS)

guitestobjs := $(DIR)/gui_test.o $(DIR)/gui_util.o
guitesths := $(DIR)/gui_util.h
guitestdeps := $(guitestobjs) $(guitesths)

sdl_net_cmd := `sdl-config --libs --cflags` -lSDL_net
sdl_gui_cmd := `sdl-config --libs --cflags` -lSDL_image
OBJS += $(RANDALLOBJS) $(guitestobjs)

all:

$(DIR)/randall: $(RANDALLDEPS)
	$(CC) $(CFLAGS) $(sdl_net_cmd) -o $@ $(RANDALLOBJS)

$(DIR)/gui_test: $(guitestdeps)
	$(CC) $(CFLAGS) $(sdl_gui_cmd) -o $@ $(guitestobjs)
