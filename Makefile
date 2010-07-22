modules := server clients testenv

# global flags
CFLAGS = -g -Wall -ansi

# extra libs
# LIBS :=

# each module will add to this
TARGETS :=
OBJS :=
#SRC :=

all:

# include module descriptions
include $(patsubst %,%/module.mk,$(modules))

# determine obj files
#OBJS := $(patsubst %.c,%.o, $(filter %.c,$(SRC)))

# go
all: $(TARGETS)

clean:
	$(RM) $(OBJS) $(TARGETS)

# Be _really_ careful with this!!
XARGS := xargs
FIND  := find
terror:
	$(FIND) . -name \*.o | $(XARGS) $(RM)
