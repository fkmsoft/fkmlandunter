dir := clients/ai/random

targets += $(dir)/random
randomobjs := $(dir)/random.o $(dir)/../../communication.o
randomhs := $(dir)/../../communication.h
randomdeps := $(randomobjs) $(randomhs)
objs += $(randomobjs)

all:

$(dir)/random: $(randomdeps)
	$(LINK.c) -o $@ $(randomobjs)
