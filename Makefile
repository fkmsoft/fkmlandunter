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
	mkdir -p $(DESTDIR)/$(PREFIX)/bin
	# servers
	install -s -m 755 server/new/server $(DESTDIR)/$(PREFIX)/bin/$(base)_srv_new
	install -s -m 755 server/old/server $(DESTDIR)/$(PREFIX)/bin/$(base)_srv_old
	# clients
	install    -m 755 clients/pyqt/client.py $(DESTDIR)/$(PREFIX)/bin/$(base)_client_py
	install -s -m 755 clients/curses/client  $(DESTDIR)/$(PREFIX)/bin/$(base)_client_curses
	install -s -m 755 clients/sdl/client     $(DESTDIR)/$(PREFIX)/bin/$(base)_client_sdl
	# links
	ln -s $(DESTDIR)/$(PREFIX)/bin/$(base)_srv_new    $(DESTDIR)/$(PREFIX)/bin/$(base)_srv
	ln -s $(DESTDIR)/$(PREFIX)/bin/$(base)_client_sdl $(DESTDIR)/$(PREFIX)/bin/$(base)
	#
	# data
	mkdir -p $(DESTDIR)/$(PREFIX)/share/fkmlandunter
	cp -r data/fkmlu                         $(DESTDIR)/$(PREFIX)/share/fkmlandunter/data
	cp    clients/sdl/example.fkmlandunterrc $(DESTDIR)/$(PREFIX)/share/fkmlandunter/data

archive:
	git archive --format=tar --prefix=fkmlandunter/ HEAD | gzip >fkmlandunter-latest.tar.gz

clean:
	@# sort is just for removing duplicates, to shorten the command line
	$(RM) $(sort $(objs)) $(targets)

# Be REALLY careful with this!!
terror:
	$(FIND) . -name \*.o | $(XARGS) $(RM)
