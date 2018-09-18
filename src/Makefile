#
# Makefile for the Tarsio C Unit-testing framework
#

TMP=/tmp/
CFLAGS=-Wall -std=c11 -pedantic -O2 -DNODEBUG
TARSIO_CFLAGS=-O0 -Wall -std=c11 -pedantic -g -I. -I${TMP}
LDFLAGS=-flto
TESTDIR=./

#ifeq(CC,vbccm68k)
#  PPFLAGS=-O=0 -noasm -I/etc/vcm68k-amigaos/include/ -I. -I${TMP} -Dmain=__tarsio_replace_main -E
#endif
#ifeq(CC,gcc)
#  PPFLAGS=-O0 -Wall -std=c11 -pedantic -I. -I${TMP} -Dmain=__tarsio_replace_main -E
#endif

.SUFFIXES:

all: tcg tsg tmg tam ttg

check: testcase_test

%.o: %.c
	$(CC) ${CFLAGS} -o $@ -c $<

%: %.o
	$(CC) -o $@ $^

#
# Produce a pre-processed file for the code to test
#
${TMP}%.p: %.c
	$(CC) ${TARSIO_CFLAGS} -Dmain=__tarsio_replace_main -E -c $< > $@

${TMP}%.sym: ${TMP}%.p
	./tcg $^ $@

.PRECIOUS: ${TMP}%_data.h
${TMP}%_data.h: ${TMP}%.sym ${TESTDIR}%_test.c
	./tsg $^ > $@

${TMP}%_mocks.c: ${TMP}%.sym ${TMP}%_data.h
	./tmg $^ > $@

${TMP}%_proxified.p: ${TMP}%.sym ${TMP}%.p
	./tam $^ > $@

${TMP}%_runner.c: ${TESTDIR}%_test.c ${TMP}%_data.h
	./ttg $^ > $@

#
# Compile the test suite to an object file
#
${TMP}%_proxified.o: ${TMP}%_proxified.p
	$(CC) ${CFLAGS} -x cpp-output -c -Wunused-function $^ -o $@

${TMP}%_test.o: ${TESTDIR}%_test.c ${TMP}%_data.h
	$(CC) ${CFLAGS} -I${TMP} -I${TESTDIR} -c $< -o $@

${TMP}%_mocks.o: ${TMP}%_mocks.c ${TMP}%_data.h
	$(CC) ${CFLAGS} -I${TMP} -I${TESTDIR} -Wunused-function -c $< -o $@

${TMP}%_runner.o: ${TMP}%_runner.c
	$(CC) ${CFLAGS} -I${TMP} -I${TESTDIR} -Wunused-function -c $^ -o $@

#
# Link the test suite object to the test case object
#
# TODO: Make tarsio.o a shared library instead
#
%_test: ${TMP}%_test.o ${TMP}%_proxified.o ${TMP}%_runner.o ${TMP}%_mocks.o tarsio.o
	$(CC) ${LDFLAGS} -o $@ $^

#
# Tarsio tool chain
#
tcg: tcg.o prototype.o file.o argument.o symbol_usage.o datatype.o symbol_cache.o
	$(CC) ${LDFLAGS} -o $@ $^

tsg: tsg.o prototype.o cpp.o symbol_cache.o file.o argument.o symbol_usage.o datatype.o
	$(CC) ${LDFLAGS} -o $@ $^

tmg: tmg.o prototype.o symbol_cache.o file.o argument.o symbol_usage.o datatype.o
	$(CC) ${LDFLAGS} -o $@ $^

tam: tam.o prototype.o symbol_cache.o file.o argument.o symbol_usage.o datatype.o
	$(CC) ${LDFLAGS} -o $@ $^

ttg: ttg.o file.o testcase.o
	$(CC) ${LDFLAGS} -o $@ $^


clean:
	${RM} *~ *.o protoparse symbolparse mockables proxify select implement *.pre *.all *.sym *.moc *.prx *.sel *.mod *.imp gts tsg tcg tmg tam ttg gent *_test ${TMP}*.pre ${TMP}*.pp* ${TMP}*_suite.c ${TMP}*_tarsio* ${TMP}*_test.o ${TMP}testcase*
