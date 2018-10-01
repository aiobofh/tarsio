# Makefile for Tarsio self-tests
Q=@

SRCDIR=../src/
INCDIR=../include/
TSTDIR=./
COVDIR=/tmp/
TMPDIR=/tmp/

CFLAGS=-O0 -Wall -std=c11 -pedantic -g -I. -I${TMPDIR} -I${INCDIR} -I${SRCDIR}

.SUFFIXES:

SUFFIXES :=
%.w:
%.v:

export Q
export PATH:=${PATH}:../src

all:
	${Q}${MAKE} --no-print-directory -C ${SRCDIR} && \
	${MAKE} --no-print-directory check

include ${INCDIR}tarsio.mk

.PHONY: check
check:: ${TSTDIR}testcase_test
	${Q}${TSTDIR}testcase_test

include ${INCDIR}coverage.mk

clean::
	${RM} *.uaem *.lnk *.info
