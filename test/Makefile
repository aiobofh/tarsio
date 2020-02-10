# Makefile for Tarsio self-tests
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
check:: $(subst .c,,$(wildcard *_test.c))
	${Q}for i in $$(ls -1 *_test); do ${EXECUTOR}$$i $(FAILCOLLECT) ; done; echo

.PHONY: clean
clean::
	${Q}${RM} -f *.uaem *.lnk *.info

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
