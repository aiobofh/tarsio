#
# Makefile for the Tarsio C Unit-testing framework
#

TMP=/tmp/
CFLAGS=-Wall -std=c11 -pedantic -O2 -DNODEBUG
TARSIO_CFLAGS=-O0 -Wall -std=c11 -pedantic -g -I. -I${TMP}
LDFLAGS=-flto
TESTDIR=./

.SUFFIXES:

.NOTPARALELL:
all: tcg tsg tmg tam ttg

check: testcase_test

%.o: %.c
	$(CC) ${CFLAGS} -o $@ -c $<

%: %.o
	$(CC) -o $@ $^

#
# Produce a pre-processed file for the code to test
#
${TMP}%_tarsio.pp: %.c
	$(CC) ${TARSIO_CFLAGS} -Dmain=__tarsio_replace_main -E -c $< > $@

${TMP}%_tarsio.sym: ${TMP}%_tarsio.pp
	./tcg $^ $@

.PRECIOUS: ${TMP}%_tarsio_data.h
${TMP}%_tarsio_data.h: ${TMP}%_tarsio.sym ${TESTDIR}%_test.c
	./tsg $^ > $@

${TMP}%_tarsio_mocks.c: ${TMP}%_tarsio.sym ${TMP}%_tarsio_data.h
	./tmg $^ > $@

${TMP}%_tarsio_proxified.pp: ${TMP}%_tarsio.sym ${TMP}%_tarsio.pp
	./tam $^ > $@

${TMP}%_tarsio_runner.c: ${TESTDIR}%_test.c ${TMP}%_tarsio_data.h
	./ttg $^ > $@

#
# Compile the test suite to an object file
#
${TMP}%_tarsio_proxified.o: ${TMP}%_tarsio_proxified.pp
	$(CC) ${CFLAGS} -x cpp-output -c -Wunused-function $^ -o $@

${TMP}%_test.o: ${TESTDIR}%_test.c ${TMP}%_tarsio_data.h
	$(CC) ${CFLAGS} -I${TMP} -I${TESTDIR} -c $< -o $@

${TMP}%_tarsio_mocks.o: ${TMP}%_tarsio_mocks.c ${TMP}%_tarsio_data.h
	$(CC) ${CFLAGS} -I${TMP} -I${TESTDIR} -Wunused-function -c $< -o $@

${TMP}%_tarsio_runner.o: ${TMP}%_tarsio_runner.c
	$(CC) ${CFLAGS} -I${TMP} -I${TESTDIR} -Wunused-function -c $^ -o $@

#
# Link the test suite object to the test case object
#
# TODO: Make tarsio.o a shared library instead
#
%_test: ${TMP}%_test.o ${TMP}%_tarsio_proxified.o ${TMP}%_tarsio_runner.o ${TMP}%_tarsio_mocks.o tarsio.o
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
	${RM} *~ *.o protoparse symbolparse mockables proxify select implement *.pre *.all *.sym *.moc *.prx *.sel *.mod *.imp gts tsg tcg tmg tam ttg gent *_test ${TMP}*.pre ${TMP}*.pp* ${TMP}*_suite.c ${TMP}*_tarsio* ${TMP}*_test.o
