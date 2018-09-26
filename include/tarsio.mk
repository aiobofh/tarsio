.SUFFIXES:

TESTSUITES=$(subst .c,,$(wildcard ${TSTDIR}*_test.c))
DATS=$(subst ${TSTDIR},${TMPDIR},$(subst _test,_data.h,${TESTSUITES}))

#
# Produce a pre-processed file for the code to test
#
.PRECIOUS: ${TMPDIR}%.p
${TMPDIR}%.p: ${SRCDIR}%.c
	${Q}${CC} ${CFLAGS} -Dmain=__tarsio_replace_main -E -c $< > $@

.PRECIOUS: ${TMPDIR}%.sym
${TMPDIR}%.sym: ${TMPDIR}%.p
	${Q}tcg $^ $@

.NOTPARALELL: ${TMPDIR}%_data.h
.PRECIOUS: ${TMPDIR}%_data.h
${TMPDIR}%_data.h: ${TMPDIR}%.sym ${TSTDIR}%_test.c
	${Q}tsg $^ > $@

.PRECIOUS: ${TMPDIR}%_mocks.c
${TMPDIR}%_mocks.c: ${TMPDIR}%.sym ${TMPDIR}%_data.h
	${Q}tmg $^ > $@

.PRECIOUS: ${TMPDIR}%_proxified.p
${TMPDIR}%_proxified.p: ${TMPDIR}%.sym ${TMPDIR}%.p
	${Q}tam $^ > $@

.PRECIOIS: ${TMPDIR}%_runner.c
${TMPDIR}%_runner.c: ${TSTDIR}%_test.c ${TMPDIR}%_data.h
	${Q}ttg $^ > $@

#
# Compile the test suite to an object file
#
.PRECIOUS: ${SRCDIR}%_proxified.o
${SRCDIR}%_proxified.o: ${TMPDIR}%_proxified.p ${TMPDIR}%_data.h

.PRECIOUS: ${TMPDIR}%_mocks.o
${TMPDIR}%_mocks.o: ${TMPDIR}%_mocks.c

.PRECIOUS: ${TMPDIR}%_runner.o
${TMPDIR}%_runner.o: ${TMPDIR}%_runner.c

.PRECIOUS: ${TMPDIR}%_test.o
${TMPDIR}%_test.o: ${TSTDIR}%_test.c ${TMPDIR}%_data.h

${TMPDIR}%.o: ${TSTDIR}%.c
	${Q}${CC} ${CFLAGS} -o $@ -c $<

${TMPDIR}%.o: ${TMPDIR}%.c
	${Q}${CC} ${CFLAGS} -o $@ -c $<

${TMPDIR}%.o: ${TMPDIR}%.p
	${Q}${CC} ${CFLAGS} -x cpp-output -c -Wunused-function $^ -o $@

${SRCDIR}%.o: ${TMPDIR}%.p
	${Q}${CC} ${CFLAGS} -x cpp-output -c -Wunused-function $^ -o $@

#
# Link the test suite object to the test case object
#
# TODO: Make tarsio.o a shared library instead
#
%_test: ${TMPDIR}%_test.o ${SRCDIR}%_proxified.o ${TMPDIR}%_runner.o ${TMPDIR}%_mocks.o ${SRCDIR}tarsio.o
	${Q}${CC} ${LDFLAGS} -o $@ $^

.PHONY: clean
clean::
	${Q}${RM} *~ ${TMPDIR}*.sym ${TMPDIR}*_data.h ${TMPDIR}*.p ${SRCDIR}*_proxified* ${TMPDIR}*_runner* ${TMPDIR}*_mocks* ${TMPDIR}*_test* *_test

#.PHONY: check
check:: ${DATS}
