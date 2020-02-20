#
# Makefile for Tarsio self-tests
#
#               _______          _____ ___        ______
#                  |      ||    |         |    | |      |
#                  |      ||    |         |    | |      |
#                  |   ___||___ |         |___ | |______|
#
#               Copyright (C) 2020 AiO Secure Teletronics
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

TSRCROOT=../src/

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

include ../include/tarsio_dist.mk

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
include ../include/coverage_dist.mk
endif
endif
endif

# Need to find the original symbols for calling real code, if required. These
# are kept for documentation purpouse... If ttm is called with --no-module
# option these symbols won't be a proble,. But on the other hand it will be
# impossible to write module-tests nor set mocks to call the original function
# if required.
argument_test${EXE}: ../src/list.o ../src/datatype.o

cpp_test${EXE}: ../src/str.o ../src/list.o ../src/file.o

testcase_test${EXE}: ../src/str.o
