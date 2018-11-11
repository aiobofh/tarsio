#
# Makefile for the Tarsio C Unit-testing framework
#

Q?=@
ifdef SASC
INCDIR:=/include/
CFLAGS=define NODEBUG define SASC optimize noicons noversion includedirectory $(INCDIR) $<
CC:=sc
LDFLAGS=noicons noversion $^ link to $@
else
ifdef VBCC
INCDIR:=../include/
CFLAGS=-DNODEBUG -O2 -c99 -c $^ -I${INCDIR} -o $@ -cpu=68000
#CFLAGS=-O2 -c99 -c $^ -I${INCDIR} -o $@ -cpu=68000
LDFLAGS=-O2 -c99 $^ -o $@
CC:=vc
else
INCDIR:=../include/
CFLAGS=-Wall -std=c11 -pedantic -O2 -g -I${INCDIR} -o $@ -c $< -DNODEBUG
#CFLAGS=-Wall -std=c11 -pedantic -O2 -g -I${INCDIR} -o $@ -c $<
LDFLAGS=-flto -o $@ $^
ifdef COV
CFLAGS+=-fprofile-arcs -ftest-coverage
LDFLAGS+=-fprofile-arcs -lgcov --coverage
endif
endif
endif

TSTDIR:=../test/

.SUFFIXES:

all: tcg tsg tmg tam ttg
	@: # Force GNU Make to be silent if nothing need to be done

check:
	$(Q)$(MAKE) && \
	$(MAKE) --no-print-directory -C ${TSTDIR}

%.o: %.c
	$(Q)$(CC) ${CFLAGS}

%:
	$(Q)$(CC) ${LDFLAGS}


#
# Tarsio tool chain
#
tcg: tcg.o list.o prototype.o file.o argument.o symbol_usage.o datatype.o symbol_cache.o

tsg: tsg.o str.o list.o prototype.o cpp.o symbol_cache.o file.o argument.o symbol_usage.o datatype.o

tmg: tmg.o list.o prototype.o symbol_cache.o file.o argument.o symbol_usage.o datatype.o

tam: tam.o list.o prototype.o symbol_cache.o file.o argument.o symbol_usage.o datatype.o

ttg: ttg.o str.o file.o testcase.o

clean::
	$(Q)${RM} *~ *.o *.so *.library *.lnk *.p *.ueam gts tsg tcg tmg tam ttg
