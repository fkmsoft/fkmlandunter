DIR := clients/sdl
TARGETS += $(DIR)/randall $(DIR)/gui_test

randallobjs := $(DIR)/randall.o $(DIR)/net_util.o $(DIR)/gui_util.o $(DIR)/../curses/communication.o
randallhs   := $(DIR)/net_util.h $(DIR)/gui_util.h $(DIR)/../curses/communication.h
randalldeps := $(randallobjs) $(randallhs)

guitestobjs := $(DIR)/gui_test.o $(DIR)/gui_util.o $(DIR)/text_util.o
guitesths   := $(DIR)/gui_util.h $(DIR)/text_util.h
guitestdeps := $(guitestobjs) $(guitesths)

sdl_cmd     := `sdl-config --libs --cflags`
sdl_net_cmd := -lSDL_net
sdl_gui_cmd := -lSDL_image -lSDL_ttf
OBJS += $(randallobjs) $(guitestobjs)

all:

$(DIR)/randall: $(randalldeps)
	$(CC) $(CFLAGS) $(sdl_cmd) $(sdl_net_cmd) $(sdl_gui_cmd) -o $@ $(randallobjs)

$(DIR)/gui_test: $(guitestdeps)
	$(CC) $(CFLAGS) $(sdl_cmd) $(sdl_gui_cmd) -o $@ $(guitestobjs)
