# Makefile for Tarsio self-tests
Q=@

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
EXECUTOR:=./
FAILCOLLECT=-c || (echo "$$i failed")
endif

export TARSIO_BIN:=../src
export Q
export PATH:=${PATH}:../src

all:
	${Q}${MAKE} --no-print-directory -C ../src/ && \
	${MAKE} --no-print-directory check

include ../include/tarsio.mk

.PHONY: check
check:: $(subst .c,,$(wildcard *_test.c))
	${Q}for i in $$(ls -1 *_test); do ${EXECUTOR}$$i $(FAILCOLLECT) ; done; echo

clean::
	${Q}${RM} *.uaem *.lnk *.info

ifndef SASC
include ../include/coverage.mk
endif
