.SUFFIXES:

${TARSIOTMP}%.o: %.c
	${Q}${CC} ${CFLAGS} -o $@ -c $<

${TARSIOTMP}%.o: ${TARSIOTMP}%.c
	${Q}${CC} ${CFLAGS} -o $@ -c $<

${TARSIOTMP}%.o: ${TARSIOTMP}%.p
	${Q}${CC} ${CFLAGS} -x cpp-output -c -Wunused-function $^ -o $@

#
# Produce a pre-processed file for the code to test
#
.PRECIOUS: ${TARSIOTMP}%.p
${TARSIOTMP}%.p: ${SRCDIR}%.c
	${Q}${CC} ${CFLAGS} -Dmain=__tarsio_replace_main -E -c $< > $@

.PRECIOUS: ${TARSIOTMP}%.sym
${TARSIOTMP}%.sym: ${TARSIOTMP}%.p
	${Q}tcg $^ $@

.NOTPARALELL: ${TARSIOTMP}%_data.h
.PRECIOUS: ${TARSIOTMP}%_data.h
${TARSIOTMP}%_data.h: ${TARSIOTMP}%.sym %_test.c
	${Q}tsg $^ > $@

.PRECIOUS: ${TARSIOTMP}%_mocks.c
${TARSIOTMP}%_mocks.c: ${TARSIOTMP}%.sym ${TARSIOTMP}%_data.h
	${Q}tmg $^ > $@

.PRECIOUS: ${TARSIOTMP}%_proxified.p
${TARSIOTMP}%_proxified.p: ${TARSIOTMP}%.sym ${TARSIOTMP}%.p
	${Q}tam $^ > $@

.PRECIOIS: ${TARSIOTMP}%_runner.c
${TARSIOTMP}%_runner.c: %_test.c ${TARSIOTMP}%_data.h
	${Q}ttg $^ > $@

#
# Compile the test suite to an object file
#
.PRECIOUS: ${TARSIOTMP}%_proxified.o
${TARSIOTMP}%_proxified.o: ${TARSIOTMP}%_proxified.p

.PRECIOUS: ${TARSIOTMP}%_test.o
${TARSIOTMP}%_test.o: %_test.c ${TARSIOTMP}%_data.h

.PRECIOUS: ${TARSIOTMP}%_mocks.o
${TARSIOTMP}%_mocks.o: ${TARSIOTMP}%_mocks.c ${TARSIOTMP}%_data.h

.PRECIOUS: ${TARSIOTMP}%_runner.o
${TARSIOTMP}%_runner.o: ${TARSIOTMP}%_runner.c

.PRECIOUS: ${TARSIOTMP}%_test.o
${TARSIOTMP}%_test.o: %_test.c

#
# Link the test suite object to the test case object
#
# TODO: Make tarsio.o a shared library instead
#
%_test: ${TARSIOTMP}%_test.o ${TARSIOTMP}%_proxified.o ${TARSIOTMP}%_runner.o ${TARSIOTMP}%_mocks.o ${SRCDIR}tarsio.o
	${Q}${CC} ${LDFLAGS} -o $@ $^

clean:
	${Q}${RM} *~ ${TARSIOTMP}*.sym ${TARSIOTMP}*_data.h ${TARSIOTMP}*.p ${TARSIOTMP}*_proxified* ${TARSIOTMP}*_runner* ${TARSIOTMP}*_mocks* ${TARSIOTMP}*_test* *_test
