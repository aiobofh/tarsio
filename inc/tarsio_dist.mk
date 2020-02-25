#
# Makefile to for building a complete check-suite using the Tarsio tool-chain
#
#               _______          _____ ___        ______
#                  |      ||    |         |    | |      |
#                  |      ||    |         |    | |      |
#                  |   ___||___ |         |___ | |______|
#
#               Copyright (C) 2020 AiO Secure Teletronics
#
# This Makefile could be included in any GNU Make system, however, it's quite
# specific.... So. Use with care.
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
# TODO: Provide some contrib files with example confiugrations for running
#       exotic compilers in various emulators from the command line.
#
MAKEFLAGS += --no-builtin-rules

.SUFFIXES:

TCHECKSUFFIX?=_test

HOSTTMPDIR:=/tmp/${USER}/tarsio/
HOSTINCDIR:=../inc/
HOSTSRCDIR:=../src/
HOSTTSTDIR:=../test/

ifdef SASC
EXE:=
CPPEXT:=.p
SRCDIR:=/src/
INCDIR:=/inc/
TSTDIR:=/test/
COVDIR=T:${USER}/tarsio/
TMPDIR=T:${USER}/tarsio/
PLACEHOLDER=T:${USER}/tarsio/placeholder

TCG=vamos -c ~/.vamosrc ${TARSIO_BIN}/tcg $(subst ${HOSTTMPDIR},${TMPDIR},$^) $(subst ${HOSTTMPDIR},${TMPDIR},$@)
TSG=vamos -c ~/.vamosrc ${TARSIO_BIN}/tsg $(subst ${HOSTTMPDIR},${TMPDIR},$^) > $@
TMG=vamos -c ~/.vamosrc ${TARSIO_BIN}/tmg $(subst ${HOSTTMPDIR},${TMPDIR},$^) > $@
TAM=vamos -c ~/.vamosrc ${TARSIO_BIN}/tam $(subst ${HOSTTMPDIR},${TMPDIR},$^) > $@
TTG=vamos -c ~/.vamosrc ${TARSIO_BIN}/ttg $(subst ${HOSTTMPDIR},${TMPDIR},$^) > $@

TMPCFLAGS=define NODEBUG define SASC optimize noicons noversion includedirectory $(TMPDIR) includedirectory $(INCDIR) includedirectory $(SRCDIR)
PPFLAGS=${TMPCFLAGS} define main=__tarsio_replace_main $(filter-out ${HOSTTMPDIR}.placeholder,$(subst ${HOSTSRCDIR},${SRCDIR},$^)) preprocessoronly; cp $(filter-out ${HOSTTMPDIR}.placeholder,$(subst .c,${CPPEXT},$^) $@; rm -f $(subst .c,${CPPEXT},$^))
CFLAGS=${TMPCFLAGS} $(subst ${HOSTTMPDIR},${TMPDIR},$(subst ${HOSTTSTDIR},${TSTDIR},$(subst ${HOSTSRCDIR},${SRCDIR},$^))) IGNORE=105
CFLAGSPP=${CFLAGS}
LDFLAGS=noicons noversion batch $(subst ${HOSTTMPDIR},${TMPDIR},$(subst ${HOSTTSTDIR},${TSTDIR},$(subst ${HOSTSRCDIR},${SRCDIR},$^))) link to $(subst ${HOSTTMPDIR},${TMPDIR},$(subst ${HOSTTSTDIR},${TSTDIR},$(subst ${HOSTSRCDIR},${SRCDIR},$@)))
CC:=sc
TESTO=mv $(subst ${HOSTTMPDIR},${HOSTTSTDIR},$@) $@
TARSIOO=mv $(subst .c,.o,$<) $@
PROXIFIEDO=mv $(subst ${HOSTSRCDIR},${HOSTTMPDIR},$@) $@
LD:=${CC}
else
ifdef VBCC
EXE:=
CPPEXT:=.i
TCG=vamos -c ~/.vamosrc ${TARSIO_BIN}/tcg $(subst ${HOSTTMPDIR},T:${USER}/tarsio/,$^) $(subst ${HOSTTMPDIR},T:${USER}/tarsio/,$@)
TSG=vamos -c ~/.vamosrc ${TARSIO_BIN}/tsg $(subst ${HOSTTMPDIR},T:${USER}/tarsio/,$^) > $@
TMG=vamos -c ~/.vamosrc ${TARSIO_BIN}/tmg $(subst ${HOSTTMPDIR},T:${USER}/tarsio/,$^) > $@
TAM=vamos -c ~/.vamosrc ${TARSIO_BIN}/tam $(subst ${HOSTTMPDIR},T:${USER}/tarsio/,$^) > $@
TTG=vamos -c ~/.vamosrc ${TARSIO_BIN}/ttg $(subst ${HOSTTMPDIR},T:${USER}/tarsio/,$^) > $@

SRCDIR:=${HOSTSRCDIR}
INCDIR:=${HOSTINCDIR}
TSTDIR:=/test/
COVDIR:=${HOSTTMPDIR}
TMPDIR:=${HOSTTMPDIR}
TMPCFLAGS=-DVBCC -O0 -c99 -g -I. -I${TMPDIR} -I${INCDIR} -I${SRCDIR}
PPFLAGS=${TMPCFLAGS} -Dmain=__tarsio_replace_main -E -c $<; cp $(subst .c,${CPPEXT},$<) $@; rm -f $(subst .c,${CPPEXT},$^)
CFLAGS=${TMPCFLAGS} -o $@ -c $<
CFLAGSPP=${TMPCFLAGS} -c $< -o $@
LDFLAGS=-c99 $^ -o $@
TESTO:=
TARSIOO:=
PROXIFIEDO:=
CC:=vc
LD:=${CC}
else
ifdef VC
EXE:=.exe
CPPEXT:=.i
TMPDIR:=T:\\${USER}\\tarsio\\
TCG=wine ${TARSIO_BIN}/tcg.exe $(subst ${HOSTTMPDIR},${TMPDIR},$^) $(subst ${HOSTTMPDIR},${TMPDIR},$@)
TSG=wine ${TARSIO_BIN}/tsg.exe $(subst ${HOSTTMPDIR},${TMPDIR},$^) > $@
TMG=wine ${TARSIO_BIN}/tmg.exe $(subst ${HOSTTMPDIR},${TMPDIR},$^) > $@
TAM=wine ${TARSIO_BIN}/tam.exe $(subst ${HOSTTMPDIR},${TMPDIR},$^) > $@
TTG=wine ${TARSIO_BIN}/ttg.exe $(subst ${HOSTTMPDIR},${TMPDIR},$^) > $@

SRCDIR:=${HOSTSRCDIR}
INCDIR:=${HOSTINCDIR}
TSTDIR:=
COVDIR:=${HOSTTMPDIR}
TMPCFLAGS=/nologo /DVC /Od /Wall /Zi /I. /I${TMPDIR} /I${INCDIR} /I${SRCDIR}
PPFLAGS=${TMPCFLAGS} /Dmain=__tarsio_replace_main /P $<; cp $(subst ${SRCDIR},,$(subst .c,${CPPEXT},$<)) $@; rm -f $(subst ${SRCDIR},,$(subst .c,${CPPEXT},$^))
CFLAGS=${TMPCFLAGS} /Fo$(subst ${HOSTTMPDIR},${TMPDIR},$@) $(subst ${HOSTTMPDIR},${TMPDIR},$<)
CFLAGSPP=${TMPCFLAGS} /Fo$(subst ${HOSTTMPDIR},${TMPDIR},$@) /Tc$(subst ${HOSTTMPDIR},${TMPDIR},$<)
LDFLAGS=/nologo /out:$@ $^
TESTO:=
TARSIOO:=
PROXIFIEDO:=
CC:=cl
LD:=link
else
CPPEXT:=.pp
TCG=tcg $^ $@
TSG=tsg $^ > $@
TMG=tmg $^ > $@
TAM=tam $^ > $@
TTG=ttg $^ > $@

SRCDIR:=${HOSTSRCDIR}
INCDIR:=${HOSTINCDIR}
TSTDIR:=
COVDIR:=${HOSTTMPDIR}
TMPDIR:=${HOSTTMPDIR}
TMPCFLAGS=-O0 -Wall -std=c11 -pedantic -g -I. -I${TMPDIR} -I${INCDIR} -I${SRCDIR}
PPFLAGS=${TMPCFLAGS} -Dmain=__tarsio_replace_main -E -c $< > $@
CFLAGS=${TMPCFLAGS} -o $@ -c $<
CFLAGSPP=${TMPCFLAGS} -x cpp-output -c -Wunused-function $< -o $@
LDFLAGS=-o $@ $^
TESTO:=
TARSIOO:=
PROXIFIEDO:=
LD=${CC}
endif
endif
endif

TESTSUITES=$(subst .c,${EXE},$(wildcard ${HOSTTSTDIR}*${TCHECKSUFFIX}.c))
DATS=$(subst ${HOSTTSTDIR},${HOSTTMPDIR},$(subst ${TCHECKSUFFIX}${EXE},_data.h,${TESTSUITES}))

info:
	echo ${TESTSUITES}
	echo ${DATS}
	echo ${CC}

.PRECIOUS: ${HOSTTMPDIR}.placeholder
${HOSTTMPDIR}.placeholder:
	${Q}mkdir -p $(dir $@) && touch $@

#
# Produce a pre-processed file for the code to test
#
.PRECIOUS: ${HOSTTMPDIR}%${CPPEXT}
${HOSTTMPDIR}%${CPPEXT}: ${HOSTSRCDIR}%.c ${HOSTTMPDIR}.placeholder
	${Q}${CC} ${PPFLAGS}

.PRECIOUS: ${HOSTTMPDIR}%.sym
${HOSTTMPDIR}%.sym: ${HOSTTMPDIR}%${CPPEXT}
	${Q}${TCG}

#.NOTPARALLEL: ${HOSTTMPDIR}%_data.h
.PRECIOUS: ${HOSTTMPDIR}%_data.h
${HOSTTMPDIR}%_data.h: ${HOSTTMPDIR}%.sym ${HOSTTSTDIR}%${TCHECKSUFFIX}.c
	${Q}${TSG}

.PRECIOUS: ${HOSTTMPDIR}%_mocks.c
${HOSTTMPDIR}%_mocks.c: ${HOSTTMPDIR}%.sym ${HOSTTMPDIR}%_data.h
	${Q}${TMG}

.PRECIOUS: ${HOSTTMPDIR}%_proxified${CPPEXT}
${HOSTTMPDIR}%_proxified${CPPEXT}: ${HOSTTMPDIR}%.sym ${HOSTTMPDIR}%${CPPEXT} ${HOSTTMPDIR}%_data.h
	${Q}${TAM}

.PRECIOUS: ${HOSTTMPDIR}%_runner.c
${HOSTTMPDIR}%_runner.c: ${HOSTTSTDIR}%${TCHECKSUFFIX}.c ${HOSTTMPDIR}%_data.h
	${Q}${TTG}

#
# Compile the test suite to an object file
#
.PRECIOUS: ${HOSTTMPDIR}%.o
${HOSTTMPDIR}%.o: ${HOSTTMPDIR}%.c
	${Q}${CC} ${CFLAGS}

.PRECIOUS: ${HOSTTMPDIR}%.o
${HOSTTMPDIR}%.o: ${HOSTTMPDIR}%${CPPEXT}
	${Q}${CC} ${CFLAGSPP}

.PRECIOUS: ${HOSTSRCDIR}%_proxified.o
${HOSTSRCDIR}%_proxified.o: ${HOSTTMPDIR}%_proxified${CPPEXT} ${HOSTTMPDIR}%_data.h
	${Q}${CC} ${CFLAGSPP}; ${PROXIFIEDO}

.PRECIOUS: ${HOSTTMPDIR}%_mocks.o
${HOSTTMPDIR}%_mocks.o: ${HOSTTMPDIR}%_mocks.c

.PRECIOUS: ${HOSTTMPDIR}%_runner.o
${HOSTTMPDIR}%_runner.o: ${HOSTTMPDIR}%_runner.c

#.NOTPARALLEL: ${HOSTTMPDIR}%${TCHECKSUFFIX}.o
.PRECIOUS: ${HOSTTMPDIR}%${TCHECKSUFFIX}.o
${HOSTTMPDIR}%${TCHECKSUFFIX}.o: ${HOSTTSTDIR}%${TCHECKSUFFIX}.c ${HOSTTMPDIR}%_data.h
	${Q}${CC} ${CFLAGS}; ${TESTO}

${HOSTTMPDIR}tarsio.o: ${HOSTSRCDIR}tarsio.c
	${Q}${CC} ${CFLAGS}; ${TARSIOO}

#
# Link the test suite object to the test case object
#
# TODO: Make tarsio.o a shared library instead
#
%${TCHECKSUFFIX}${EXE}: ${HOSTTMPDIR}%${TCHECKSUFFIX}.o ${HOSTSRCDIR}%_proxified.o ${HOSTTMPDIR}%_runner.o ${HOSTTMPDIR}%_mocks.o ${HOSTTMPDIR}tarsio.o
	${Q}${LD} ${LDFLAGS}

.PHONY: clean
clean::
	${Q}${RM} -f *~ ${HOSTTMPDIR}*.sym ${HOSTTMPDIR}*_data.h ${HOSTTMPDIR}*_data.o ${HOSTTMPDIR}*.p ${HOSTTMPDIR}*.pp ${HOSTTMPDIR}*.i ${HOSTTMPDIR}*_proxified* ${HOSTSRCDIR}*_proxified* ${HOSTTMPDIR}*_runner* ${HOSTTMPDIR}*_mocks* ${HOSTTMPDIR}*${TCHECKSUFFIX}* *${TCHECKSUFFIX}${EXE} *.o ${HOSTTMPDIR}file*.asm ${HOSTTMPDIR}file*.o *.i ${HOSTTMPDIR}tarsio.o ${HOSTTMPDIR}.placeholder

.PHONY: check
check:: ${DATS}
