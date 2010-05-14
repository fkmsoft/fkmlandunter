DIR := clients/sdl
TARGETS += $(DIR)/resolve $(DIR)/randall

RESOLVEOBJS := $(DIR)/resolve.o
RESOLVEHS :=
RESOLVEDEPS := $(RESOLVEOBJS) $(RESOLVEHS)

RANDALLOBJS := $(DIR)/randall.o $(DIR)/sdl_util.o
RANDALLHS := $(DIR)/sdl_util.h
RANDALLDEPS := $(RANDALLOBJS) $(RANDALLHS)

SDL_CMD := `sdl-config --libs --cflags` -lSDL_net
OBJS += $(RESOLVEOBJS) $(RANDALLOBJS)

$(DIR)/resolve: $(RESOLVEDEPS)
	$(CC) $(CFLAGS) $(SDL_CMD) -o $@ $(RESOLVEOBJS)

$(DIR)/randall: $(RANDALLDEPS)
	$(CC) $(CFLAGS) $(SDL_CMD) -o $@ $(RANDALLOBJS)
