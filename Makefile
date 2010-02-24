all:
	$(MAKE) -C servex
	$(MAKE) -C clientex

clean:
	$(MAKE) -C servex $@
	$(MAKE) -C clientex $@
