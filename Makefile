MODULES := server clients testenv
# find stuff
# CFLAGS += $(patsubst %,-I,$(MODULES))
CFLAGS = -g -Wall -ansi

# extra libs
# LIBS :=

# each module will add to this
TARGETS :=
OBJS :=
SRC :=

dummy: all

# include module descriptions
include $(patsubst %,%/module.mk,$(MODULES))

# determine obj files
#OBJS := $(patsubst %.c,%.o, $(filter %.c,$(SRC)))

# go
all: $(TARGETS)

clean:
	rm -rf $(OBJS) $(TARGETS)
