dir := testenv
targets += $(dir)/testenv $(dir)/server $(dir)/random \
		   $(dir)/nserv $(dir)/sdl_randl

testenvobjs = $(dir)/testenv.o
objs += $(testenvobjs)

all:

$(dir)/testenv: $(testenvobjs)
	$(LINK.c) -o $@ $^

$(dir)/server: server/old/server
	if [ ! -L $@ ]; then ln -s ../$< $@; fi

$(dir)/nserv: server/new/server
	if [ ! -L $@ ]; then ln -s ../$< $@; fi

$(dir)/random: clients/ai/random/random
	if [ ! -L $@ ]; then ln -s ../$< $@; fi

$(dir)/sdl_randl: clients/sdl/randall
	if [ ! -L $@ ]; then ln -s ../$< $@; fi
