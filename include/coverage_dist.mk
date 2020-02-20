#
# Makefile to get code coverage from checks run.
#
#               _______          _____ ___        ______
#                  |      ||    |         |    | |      |
#                  |      ||    |         |    | |      |
#                  |   ___||___ |         |___ | |______|
#
#               Copyright (C) 2020 AiO Secure Teletronics
#
# This Makefile provide a few targets that makes it possible to know your code
# and branch coverage.
#
#  This file is part of Tarsio.
#
#  Tarsio is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  Tarsio is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with Tarsio.  If not, see <https://www.gnu.org/licenses/>.
#

MAKEFLAGS += --no-builtin-rules

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
	${Q}${RM} -f ${BRANCHES_COV} ${LINES_COV} ${COVERAGE_XML} ${SRCDIR}*.gcda ${SRCDIR}*.gcno

.PHONY: check
check::
	${Q}$(MAKE) -r --no-print-directory output_coverage
