YFLAGS := -d

targets := simple server

simpleobjs := simple.o
simpledeps := $(simpleobjs) -lfl

serverobjs := servy.o servl.o
serverdeps := $(serverobjs) server.c server.h

all: $(targets)

simple: $(simpledeps)

server: $(serverdeps)

clean:
	$(RM) $(targets) $(simpleobjs) $(serverobjs) y.tab.h
