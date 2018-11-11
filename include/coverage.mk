.SUFFIXES:

SUFFIXES :=
%.w:
%.v:

CFLAGS+=-fprofile-arcs -ftest-coverage
LDFLAGS+=-fprofile-arcs -lgcov --coverage
TMPDIR?=.
COVEX?=
COVERAGE_XML:=${TSTDIR}/coverage.xml
LINES_COV:=${TMPDIR}/lines.cov
BRANCHES_COV:=${TMPDIR}/branches.cov

#FILTER=-e 'tarsio.c' -e '/usr.*' -e '.*_test.c' -e 'tarsio.h'

${COVERAGE_XML}: check
	${Q}gcovr --object-directory=$(realpath ${SRCDIR}) -r ${SRCDIR} ${FILTER} ${COVEX} -x > $@

.NOTPARALLEL: ${LINES_COV}
${LINES_COV}: $(subst .c,,$(wildcard ${TSTDIR}/*_test.c))
	${Q}gcovr --object-directory=$(realpath ${SRCDIR}) -r ${SRCDIR} ${FILTER} ${COVEX} | \
	egrep -v '^File' | \
	egrep -v '^-' | \
	egrep -v '^Directory' | \
	grep -v 'GCC Code' | \
	grep -v '100%' | \
	grep -v "\-\-\%" > $@; true

.NOTPARALLEL: ${BRANCHES_COV}
${BRANCHES_COV}: $(subst .c,,$(wildcard ${TSTDIR}/*_test.c))
	${Q}gcovr --object-directory=$(realpath ${SRCDIR}) -r ${SRCDIR} ${FILTER} ${COVEX} -b | \
	egrep -v '^File' | \
	egrep -v '^-' | \
	egrep -v '^Directory' | \
	grep -v 'GCC Code' | \
	grep -v '100%' | \
	grep -v "\-\-\%" > $@; true

.PHONY: output_coverage
output_coverage: ${LINES_COV} ${BRANCHES_COV}
	${Q}test -s ${LINES_COV} && \
	echo "Lines not covered:" >&2 && \
	cat ${LINES_COV} >&2 && echo >&2; \
	test -s ${BRANCHES_COV} && \
	echo "Branches not covered:" >&2; \
	cat ${BRANCHES_COV} >&2

.PHONY: clean
clean::
	${Q}${RM} ${BRANCHES_COV} ${LINES_COV} ${COVERAGE_XML} ${SRCDIR}*.gcda ${SRCDIR}*.gcno

.PHONY: check
check::
	${Q}$(MAKE) -r --no-print-directory output_coverage
