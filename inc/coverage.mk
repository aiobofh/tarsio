#
# Generic GNU Make include file for automatic code coverage checks.
#
#               _______          _____ ___        ______
#                  |      ||    |         |    | |      |
#                  |      ||    |         |    | |      |
#                  |   ___||___ |         |___ | |______|
#
#             Copyright (C) 2020-2022 AiO Secure Teletronics
#
# This Makefile could be included in any GNU Make system.
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

CFLAGS+=-fprofile-arcs -ftest-coverage -fno-exceptions -fno-inline
LDFLAGS+=-fprofile-arcs -lgcov --coverage -fno-exceptions -fno-inline
COVERAGE_XML:=${TTESTROOT}coverage.xml
LINES_COV:=${TTMPROOT}lines.cov
BRANCHES_COV:=${TTMPROOT}branches.cov
FILTER=-e ".*${TCHECKSUFFIX}.c" -e ".*$tarsio.c"

GCOVR:=$(shell which gcovr)

ifeq (,${GCOVR})
	GCOVR=$(error Tarsio coverage.mk require gcovr installed)
endif

ifndef TCHECKSUFFIX
	$(error TCHECKSUFFIX is not set)
endif

xml:: ${COVERAGE_XML}

${COVERAGE_XML}:
	${Q}${GCOVR} ${FILTER} ${COVEX} -x > $@

.NOTPARALLEL: ${LINES_COV}
${LINES_COV}: $(subst .c,,$(wildcard ${TTESTROOT}/*${TCHECKSUFFIX}.c))
	${Q}${GCOVR} ${FILTER} ${COVEX} | \
	egrep -v '^File' | \
	egrep -v '^-' | \
	egrep -v '^Directory' | \
	grep -v 'GCC Code' | \
	grep -v '100%' | \
	grep -v "\-\-\%" > $@; true

.NOTPARALLEL: ${BRANCHES_COV}
${BRANCHES_COV}: $(subst .c,,$(wildcard ${TTESTROOT}/*${TCHECKSUFFIX}.c))
	${Q}${GCOVR} ${FILTER} ${COVEX} -b | \
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
