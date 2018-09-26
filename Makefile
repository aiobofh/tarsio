#
# Makefile for Tarsio
#

all:
	$(MAKE) -C src

check:
	$(MAKE) -C test

clean:
	$(MAKE) -C src clean
	$(MAKE) -C test clean
	$(RM) *~ include/*~
