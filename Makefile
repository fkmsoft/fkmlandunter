# some binaries
XARGS := xargs
FIND  := find

# global flags
CFLAGS = -g -Wall -ansi

# each module will add to this
targets :=
objs    :=

all:

# include module descriptions
modules := server clients testenv
include $(patsubst %,%/module.mk,$(modules))

# go
all: $(targets)

clean:
	$(RM) $(objs) $(targets)

# Be REALLY careful with this!!
terror:
	$(FIND) . -name \*.o | $(XARGS) $(RM)
