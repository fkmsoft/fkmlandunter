DIR := clients/sdl
TARGETS += $(DIR)/resolve
RESOLVEOBJS := $(DIR)/resolve.o
RESOLVEHS :=
RESOLVEDEPS := $(RESOLVEOBJS) $(RESOLVEHS)
SDL_CMD := `sdl-config --libs --cflags` -lSDL_net
OBJS += $(RESOLVEOBJS)

$(DIR)/resolve: $(RESOLVEDEPS)
	$(CC) $(CFLAGS) $(SDL_CMD) -o $@ $(RESOLVEOBJS)
