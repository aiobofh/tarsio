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
check:: str_test list_test cpp_test testcase_test datatype_test symbol_usage_test argument_test
	${Q}${EXECUTOR}str_test -c; \
	${EXECUTOR}list_test -c; \
	${EXECUTOR}cpp_test -c; \
	${EXECUTOR}testcase_test -c; \
	${EXECUTOR}datatype_test -c; \
	${EXECUTOR}symbol_usage_test -c; \
	${EXECUTOR}argument_test

clean::
	${Q}${RM} *.uaem *.lnk *.info

include ../include/coverage.mk
