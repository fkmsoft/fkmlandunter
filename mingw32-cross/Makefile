CC := i486-mingw32-gcc
STRIP := /usr/i486-mingw32/bin/strip
MKDIR_P := mkdir -p
ZIP := zip
CP := cp

CFLAGS := `/usr/i486-mingw32/bin/sdl-config --cflags`
LDFLAGS := `/usr/i486-mingw32/bin/sdl-config --static-libs`

dir := ../clients/sdl/

randallobjs := randall.o net_util.o gui_util.o parse.o
randallhs   := $(dir)/net_util.h $(dir)/gui_util.h $(dir)/../parse.h
randalldeps := $(randallobjs) $(randallhs)

guitestobjs := gui_test.o gui_util.o text_util.o
guitesths   := $(dir)/gui_util.h $(dir)/text_util.h
guitestdeps := $(guitestobjs) $(guitesths)

clientobjs  := client.o net_util.o gui_util.o parse.o config.o text_util.o
clienths    := $(randallhs) $(dir)/config.h $(dir)/text_util.h
clientdeps  := $(clientobjs) $(clienths)

objs := $(guitestobjs) $(randallobjs) $(clientobjs)
targets := gui_test.exe randall.exe client.exe

ZIPDIR := fkmlandunter-w32

all: $(targets)

zip: all
	$(MKDIR_P) $(ZIPDIR)
	$(CP) -r ../data/fkmlu $(ZIPDIR)/data
	$(CP) $(targets) $(ZIPDIR)
	$(ZIP) -r $(ZIPDIR).zip $(ZIPDIR)

clean:
	$(RM) $(objs) $(targets) $(ZIPDIR).zip
	$(RM) -r $(ZIPDIR)

gui_test.exe: $(guitestdeps)
	$(CC) --static -o $@ $(guitestobjs) $(LDFLAGS) -lSDL_ttf -lSDL_image -lfreetype -lpng -lz
	$(STRIP) $@

randall.exe: $(randalldeps)
	$(CC) --static -o $@ $(randallobjs) $(LDFLAGS) -lSDL_ttf -lSDL_image -lfreetype -lpng -lz \
		-lSDL_net -lwsock32
	$(STRIP) $@

client.exe: $(clientdeps)
	$(CC) --static -o $@ $(clientobjs) $(LDFLAGS) -lSDL_ttf -lSDL_image -lfreetype -lpng -lz \
		-lSDL_net -lwsock32
	$(STRIP) $@

parse.o: $(dir)/../parse.c
	$(COMPILE.c) $< -o $@

%.o: $(dir)/%.c
	$(COMPILE.c) $< -o $@
