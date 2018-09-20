# Makefile for Tarsio self-tests
Q=@

SRCDIR=../src/
INCDIR=../include/

TARSIOTMP=/tmp/

CFLAGS=-O0 -Wall -std=c11 -pedantic -g -I. -I${TARSIOTMP} -I${INCDIR} -I${SRCDIR}

export Q
export PATH:=${PATH}:../src

all:
	${Q}${MAKE} --no-print-directory -C ${SRCDIR} && \
	${MAKE} --no-print-directory check

include ${INCDIR}/tarsio.mk

check: testcase_test
	${Q}./testcase_test
