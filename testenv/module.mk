DIR := testenv
TARGETS += $(DIR)/testenv $(DIR)/server $(DIR)/random $(DIR)/ret
TESTENVOBJS = $(DIR)/testenv.o
RETOBJS = $(DIR)/ret.o
OBJS += $(TESTENVOBJS) $(RETOBJS)

$(DIR)/testenv: $(TESTENVOBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(DIR)/ret: $(RETOBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(DIR)/server: server/server
	rm -f $@
	ln -s ../$< $(DIR)

$(DIR)/random: clients/ai/random/random
	rm -f $@
	ln -s ../$< $(DIR)
