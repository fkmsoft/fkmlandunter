CC := i486-mingw32-gcc
STRIP := /usr/i486-mingw32/bin/strip
MKDIR_P := mkdir -p
ZIP := zip
CP := cp

CFLAGS := `/usr/i486-mingw32/bin/sdl-config --cflags`
ldflags_static := `/usr/i486-mingw32/bin/sdl-config --static-libs`
ldflags_dll := `/usr/i486-mingw32/bin/sdl-config --libs`

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
targets := gui_test.exe randall.exe client.exe gui_test_static.exe randall_static.exe client_static.exe

ZIPDIR := fkmlandunter-w32

DLLDIR := /usr/i486-mingw32/bin/
dlls := libfreetype-6.dll libpng14.dll SDL.dll SDL_image.dll SDL_net.dll SDL_ttf.dll zlib1.dll

all: $(targets)

zip: all
	$(MKDIR_P) $(ZIPDIR)
	$(RM) -r $(ZIPDIR)/data
	$(CP) -r ../data/fkmlu $(ZIPDIR)/data
	$(CP) $(targets) $(ZIPDIR)
	$(CP) $(addprefix $(DLLDIR)/,$(dlls)) $(ZIPDIR)
	$(ZIP) -r $(ZIPDIR).zip $(ZIPDIR)

clean:
	$(RM) $(objs) $(targets) $(ZIPDIR).zip
	$(RM) -r $(ZIPDIR)

gui_test_static.exe: $(guitestdeps)
	$(CC) --static -o $@ $(guitestobjs) $(ldflags_static) -lSDL_ttf -lSDL_image -lfreetype -lpng -lz
	$(STRIP) $@

randall_static.exe: $(randalldeps)
	$(CC) --static -o $@ $(randallobjs) $(ldflags_static) -lSDL_ttf -lSDL_image -lfreetype -lpng -lz \
		-lSDL_net -lwsock32
	$(STRIP) $@

client_static.exe: $(clientdeps)
	$(CC) --static -o $@ $(clientobjs) $(ldflags_static) -lSDL_ttf -lSDL_image -lfreetype -lpng -lz \
		-lSDL_net -lwsock32
	$(STRIP) $@

gui_test.exe: $(guitestdeps)
	$(CC) -o $@ $(guitestobjs) $(ldflags_dll) -lSDL_ttf -lSDL_image
	$(STRIP) $@

randall.exe: $(randalldeps)
	$(CC) -o $@ $(randallobjs) $(ldflags_dll) -lSDL_ttf -lSDL_image -lSDL_net
	$(STRIP) $@

client.exe: $(clientdeps)
	$(CC) -o $@ $(clientobjs) $(ldflags_dll) -lSDL_ttf -lSDL_image -lSDL_net
	$(STRIP) $@

parse.o: $(dir)/../parse.c
	$(COMPILE.c) $< -o $@

%.o: $(dir)/%.c
	$(COMPILE.c) $< -o $@
