DIR := testenv
TARGETS += $(DIR)/testenv $(DIR)/server $(DIR)/random \
		   $(DIR)/nserv $(DIR)/sdl_randl
TESTENVOBJS = $(DIR)/testenv.o
OBJS += $(TESTENVOBJS) $(RETOBJS)

$(DIR)/testenv: $(TESTENVOBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(DIR)/server: server/old/server
	rm -f $@
	ln -s ../$< $(DIR)

$(DIR)/nserv: server/new/server
	rm -f $@
	ln -s ../$< $@

$(DIR)/random: clients/ai/random/random
	rm -f $@
	ln -s ../$< $(DIR)
$(DIR)/sdl_randl: clients/sdl/randall
	rm -f $@
	ln -s ../$< $@
