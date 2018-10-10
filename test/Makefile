# Makefile for Tarsio self-tests
Q=@

.SUFFIXES:

SUFFIXES :=
%.w:
%.v:

ifdef SASC
SEP:=
EXECUTOR=vamos -c ~/.vamosrc ${SEP}
else
EXECUTOR:=./
endif

export TARSIO_BIN:=../src
export Q
export PATH:=${PATH}:../src

all:
	${Q}${MAKE} --no-print-directory -C ../src/ && \
	${MAKE} --no-print-directory check

include ../include/tarsio.mk

.PHONY: check
check:: testcase_test
	${Q}${EXECUTOR}testcase_test

# include ../include/coverage.mk

clean::
	${Q}${RM} *.uaem *.lnk *.info
