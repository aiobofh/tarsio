CFLAGS+=-fprofile-arcs -ftest-coverage
LDFLAGS+=-fprofile-arcs -lgcov --coverage
COVERAGE_XML:=${TTESTROOT}coverage.xml
LINES_COV:=${TTMPROOT}lines.cov
BRANCHES_COV:=${TTMPROOT}branches.cov
FILTER=-e ".*_check.c"

ifndef TCHECKSUFFIX
	$(error TCHECKSUFFIX is not set)
endif

${COVERAGE_XML}:
	${Q}gcovr ${FILTER} ${COVEX} -x > $@

.NOTPARALLEL: ${LINES_COV}
${LINES_COV}: $(subst .c,,$(wildcard ${TTESTROOT}/*${TCHECKSUFFIX}.c))
	${Q}gcovr ${FILTER} ${COVEX} | \
	egrep -v '^File' | \
	egrep -v '^-' | \
	egrep -v '^Directory' | \
	grep -v 'GCC Code' | \
	grep -v '100%' | \
	grep -v "\-\-\%" > $@; true

.NOTPARALLEL: ${BRANCHES_COV}
${BRANCHES_COV}: $(subst .c,,$(wildcard ${TTESTROOT}/*${TCHECKSUFFIX}.c))
	${Q}gcovr ${FILTER} ${COVEX} -b | \
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

check::
	${Q}${MAKE} -r --no-print-directory output_coverage

clean::
	${Q}${RM} -f ${LINES_COV} ${BRANCHES_COV} ${COVERAGE_XML} ${TTESTROOT}*.gcda ${TTESTROOT}*.gcno
