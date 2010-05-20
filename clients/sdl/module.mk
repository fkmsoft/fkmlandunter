DIR := clients/sdl
TARGETS += $(DIR)/randall

RANDALLOBJS := $(DIR)/randall.o $(DIR)/sdl_util.o
RANDALLHS := $(DIR)/sdl_util.h
RANDALLDEPS := $(RANDALLOBJS) $(RANDALLHS)

SDL_CMD := `sdl-config --libs --cflags` -lSDL_net
OBJS += $(RANDALLOBJS)

$(DIR)/randall: $(RANDALLDEPS)
	$(CC) $(CFLAGS) $(SDL_CMD) -o $@ $(RANDALLOBJS)
