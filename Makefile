modules := server clients testenv

# global flags
CFLAGS = -g -Wall -ansi

# each module will add to this
targets :=
objs    :=

all:

# include module descriptions
include $(patsubst %,%/module.mk,$(modules))

# go
all: $(targets)

clean:
	$(RM) $(objs) $(targets)

# Be REALLY careful with this!!
XARGS := xargs
FIND  := find
terror:
	$(FIND) . -name \*.o | $(XARGS) $(RM)
