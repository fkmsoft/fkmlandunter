# some binaries
XARGS := xargs
FIND  := find

# for installation
base := fkmlandunter
PREFIX ?= /usr/local

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

install: all
	# binaries
	mkdir -p $(PREFIX)/bin
	# servers
	install -s -m 755 server/new/server $(PREFIX)/bin/$(base)_srv_new
	install -s -m 755 server/old/server $(PREFIX)/bin/$(base)_srv_old
	# clients
	install    -m 755 clients/pyqt/client.py $(PREFIX)/bin/$(base)_client_py
	install -s -m 755 clients/curses/client  $(PREFIX)/bin/$(base)_client_curses
	install -s -m 755 clients/sdl/client     $(PREFIX)/bin/$(base)_client_sdl
	# links
	ln -s $(PREFIX)/bin/$(base)_srv_new $(PREFIX)/bin/$(base)_srv
	ln -s $(PREFIX)/bin/$(base)_client_sdl $(PREFIX)/bin/$(base)
	#
	# data
	mkdir -p $(PREFIX)/share/fkmlandunter
	cp -r data/fkmlu $(PREFIX)/share/fkmlandunter/data

clean:
	$(RM) $(objs) $(targets)

# Be REALLY careful with this!!
terror:
	$(FIND) . -name \*.o | $(XARGS) $(RM)
