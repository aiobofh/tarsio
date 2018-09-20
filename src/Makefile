#
# Makefile for the Tarsio C Unit-testing framework
#

Q?=@
INCDIR=../include/
CFLAGS=-Wall -std=c11 -pedantic -O2 -g -DNODEBUG -I${INCDIR}
LDFLAGS=-flto

.SUFFIXES:

all: tcg tsg tmg tam ttg tarsio.o
	@: # Force GNU Make to be silent if nothing need to be done

check:
	$(Q)$(MAKE) && \
	$(MAKE) --no-print-directory -C ../test/

%.o: %.c
	$(Q)$(CC) ${CFLAGS} -o $@ -c $<

%: %.o
	$(Q)$(CC) -o $@ $^


#
# Tarsio tool chain
#
tcg: tcg.o prototype.o file.o argument.o symbol_usage.o datatype.o symbol_cache.o
	$(Q)$(CC) ${LDFLAGS} -o $@ $^

tsg: tsg.o prototype.o cpp.o symbol_cache.o file.o argument.o symbol_usage.o datatype.o
	$(Q)$(CC) ${LDFLAGS} -o $@ $^

tmg: tmg.o prototype.o symbol_cache.o file.o argument.o symbol_usage.o datatype.o
	$(Q)$(CC) ${LDFLAGS} -o $@ $^

tam: tam.o prototype.o symbol_cache.o file.o argument.o symbol_usage.o datatype.o
	$(Q)$(CC) ${LDFLAGS} -o $@ $^

ttg: ttg.o file.o testcase.o
	$(Q)$(CC) ${LDFLAGS} -o $@ $^

clean:
	$(Q)${RM} *~ *.o gts tsg tcg tmg tam ttg *_test
