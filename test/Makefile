#
# Makefile for Tarsio self-tests
#
#               _______          _____ ___        ______
#                  |      ||    |         |    | |      |
#                  |      ||    |         |    | |      |
#                  |   ___||___ |         |___ | |______|
#
#                    Copyleft AiO Secure Teletronics
#
# This Makefile will compile (and run) the self-tests for Tarsio.
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

Q:=@

.SUFFIXES:

SUFFIXES :=
%.w:
%.v:

ifdef SASC
SEP:=
EXECUTOR=vamos -c ~/.vamosrc ${SEP}
FAILCOLLECT:=COMPACT
else
ifdef VBCC
SEP:=
EXECUTOR=vamos -c ~/.vamosrc ${SEP}
FAILCOLLECT:=COMPACT
else
EXECUTOR:=./
FAILCOLLECT=-c || (echo "$$i failed")
COV=1
export COV
endif
endif

export TARSIO_BIN:=../src
export PATH:=${PATH}:../src
export SASC:=${SASC}
export VBCC:=${VBCC}

ifndef VERBOSE
#.SILENT:
endif

.PHONY: all
all:
	${Q}${MAKE} --no-print-directory -C ../src/ && \
	${MAKE} --no-print-directory check

.PHONY: tools
tools:
	${Q}${MAKE} --no-print-directory -C ../src/

include ../include/tarsio.mk

.PHONY: check
check:: $(subst .c,${EXE},$(wildcard *_test.c))
	${Q}for i in $$(ls -1 *_test); do ${EXECUTOR}$$i $(FAILCOLLECT) ; done; echo

.PHONY: clean
clean::
	${Q}${RM} -f *.uaem *.lnk *.info *.pdb

.PHONY: clean-all
clean-all:
	${Q}${MAKE} --no-print-directory clean && \
	${MAKE} --no-print-directory -C ../src/ clean

ifndef SASC
ifndef VBCC
ifndef VC
include ../include/coverage.mk
endif
endif
endif
