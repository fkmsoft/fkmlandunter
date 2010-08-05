dir := testenv
targets += $(dir)/testenv $(dir)/server $(dir)/random \
		   $(dir)/nserv $(dir)/sdl_randl

testenvobjs := $(dir)/testenv.o
objs += $(testenvobjs)

all:

$(dir)/testenv: $(testenvobjs)
	@echo "  CCLD" $@
	@$(LINK.c) -o $@ $^

$(dir)/server: server/old/server
	@echo " SYMLN" $@
	@if [ ! -L $@ ]; then ln -s ../$< $@; fi

$(dir)/nserv: server/new/server
	@echo " SYMLN" $@
	@if [ ! -L $@ ]; then ln -s ../$< $@; fi

$(dir)/random: clients/ai/random/random
	@echo " SYMLN" $@
	@if [ ! -L $@ ]; then ln -s ../$< $@; fi

$(dir)/sdl_randl: clients/sdl/randall
	@echo " SYMLN" $@
	@if [ ! -L $@ ]; then ln -s ../$< $@; fi
