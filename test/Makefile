# Makefile for Tarsio self-tests
Q:=@

COV=1
export COV

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
endif
endif

export TARSIO_BIN:=../src
export PATH:=${PATH}:../src
export SASC:=${SASC}
export VBCC:=${VBCC}

ifndef VERBOSE
#.SILENT:
endif

all:
	${Q}${MAKE} --no-print-directory -C ../src/ && \
	${MAKE} --no-print-directory check

tools:
	${Q}${MAKE} --no-print-directory -C ../src/

include ../include/tarsio.mk

.PHONY: check
check:: $(subst .c,,$(wildcard *_test.c))
	${Q}for i in $$(ls -1 *_test); do ${EXECUTOR}$$i $(FAILCOLLECT) ; done; echo

clean::
	${Q}${RM} *.uaem *.lnk *.info

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
