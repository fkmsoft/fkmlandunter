DIR := testenv
TARGETS += $(DIR)/testenv $(DIR)/server $(DIR)/random
TESTENVOBJS = $(DIR)/testenv.o
OBJS += $(TESTENVOBJS)

$(DIR)/testenv: $(TESTENVOBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(DIR)/server: server/server
	rm -f $@
	ln -s ../$< $(DIR)

$(DIR)/random: clients/ai/random/random
	rm -f $@
	ln -s ../$< $(DIR)
