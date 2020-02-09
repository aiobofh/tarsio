.SUFFIXES:

HOSTTMPDIR:=/tmp/
HOSTINCDIR:=../include/
HOSTSRCDIR:=../src/
HOSTTSTDIR:=../test/

ifdef SASC
EXE:=
CPPEXT:=.p
SRCDIR:=/src/
INCDIR:=/include/
TSTDIR:=/test/
COVDIR=T:
TMPDIR=T:

TCG=vamos -c ~/.vamosrc ${TARSIO_BIN}/tcg $(subst ${HOSTTMPDIR},${TMPDIR},$^) $(subst ${HOSTTMPDIR},${TMPDIR},$@)
TSG=vamos -c ~/.vamosrc ${TARSIO_BIN}/tsg $(subst ${HOSTTMPDIR},${TMPDIR},$^) > $@
TMG=vamos -c ~/.vamosrc ${TARSIO_BIN}/tmg $(subst ${HOSTTMPDIR},${TMPDIR},$^) > $@
TAM=vamos -c ~/.vamosrc ${TARSIO_BIN}/tam $(subst ${HOSTTMPDIR},${TMPDIR},$^) > $@
TTG=vamos -c ~/.vamosrc ${TARSIO_BIN}/ttg $(subst ${HOSTTMPDIR},${TMPDIR},$^) > $@

TMPCFLAGS=define NODEBUG define SASC optimize noicons noversion includedirectory $(TMPDIR) includedirectory $(INCDIR) includedirectory $(SRCDIR)
PPFLAGS=${TMPCFLAGS} define main=__tarsio_replace_main $(subst ${HOSTSRCDIR},${SRCDIR},$^) preprocessoronly; cp $(subst .c,${CPPEXT},$^) $@; rm $(subst .c,${CPPEXT},$^)
CFLAGS=${TMPCFLAGS} $(subst ${HOSTTMPDIR},${TMPDIR},$(subst ${HOSTTSTDIR},${TSTDIR},$(subst ${HOSTSRCDIR},${SRCDIR},$^))) IGNORE=105
CFLAGSPP=${CFLAGS}
LDFLAGS=noicons noversion $(subst ${HOSTTMPDIR},${TMPDIR},$(subst ${HOSTTSTDIR},${TSTDIR},$(subst ${HOSTSRCDIR},${SRCDIR},$^))) link to $(subst ${HOSTTMPDIR},${TMPDIR},$(subst ${HOSTTSTDIR},${TSTDIR},$(subst ${HOSTSRCDIR},${SRCDIR},$@)))
CC:=sc
TESTO=mv $(subst ${HOSTTMPDIR},${HOSTTSTDIR},$@) $@
PROXIFIEDO=mv $(subst ${HOSTSRCDIR},${HOSTTMPDIR},$@) $@
LD:=${CC}
else
ifdef VBCC
EXE:=
CPPEXT:=.i
TCG=vamos -c ~/.vamosrc ${TARSIO_BIN}/tcg $(subst ${HOSTTMPDIR},T:,$^) $(subst ${HOSTTMPDIR},T:,$@)
TSG=vamos -c ~/.vamosrc ${TARSIO_BIN}/tsg $(subst ${HOSTTMPDIR},T:,$^) > $@
TMG=vamos -c ~/.vamosrc ${TARSIO_BIN}/tmg $(subst ${HOSTTMPDIR},T:,$^) > $@
TAM=vamos -c ~/.vamosrc ${TARSIO_BIN}/tam $(subst ${HOSTTMPDIR},T:,$^) > $@
TTG=vamos -c ~/.vamosrc ${TARSIO_BIN}/ttg $(subst ${HOSTTMPDIR},T:,$^) > $@

SRCDIR:=${HOSTSRCDIR}
INCDIR:=${HOSTINCDIR}
TSTDIR:=/test/
COVDIR:=${HOSTTMPDIR}
TMPDIR:=${HOSTTMPDIR}
TMPCFLAGS=-DVBCC -O0 -c99 -g -I. -I${TMPDIR} -I${INCDIR} -I${SRCDIR}
PPFLAGS=${TMPCFLAGS} -Dmain=__tarsio_replace_main -E -c $<; cp $(subst .c,${CPPEXT},$<) $@; rm $(subst .c,${CPPEXT},$^)
CFLAGS=${TMPCFLAGS} -o $@ -c $<
CFLAGSPP=${TMPCFLAGS} -c $< -o $@
#LDFLAGS=-o $@ $^
LDFLAGS=-c99 $^ -o $@
TESTO:=
PROXIFIEDO:=
CC:=vc
LD:=${CC}
else
ifdef VC
EXE:=.exe
CPPEXT:=.i
TMPDIR:=T:\\
TCG=wine ${TARSIO_BIN}/tcg.exe $(subst ${HOSTTMPDIR},${TMPDIR},$^) $(subst ${HOSTTMPDIR},${TMPDIR},$@)
TSG=wine ${TARSIO_BIN}/tsg.exe $(subst ${HOSTTMPDIR},${TMPDIR},$^) > $@
TMG=wine ${TARSIO_BIN}/tmg.exe $(subst ${HOSTTMPDIR},${TMPDIR},$^) > $@
TAM=wine ${TARSIO_BIN}/tam.exe $(subst ${HOSTTMPDIR},${TMPDIR},$^) > $@
TTG=wine ${TARSIO_BIN}/ttg.exe $(subst ${HOSTTMPDIR},${TMPDIR},$^) > $@

SRCDIR:=${HOSTSRCDIR}
INCDIR:=${HOSTINCDIR}
TSTDIR:=
COVDIR:=${HOSTTMPDIR}
TMPCFLAGS=/nologo /Od /Wall /Zi /I. /I${TMPDIR} /I${INCDIR} /I${SRCDIR}
PPFLAGS=${TMPCFLAGS} /Dmain=__tarsio_replace_main /P $<; cp $(subst ${SRCDIR},,$(subst .c,${CPPEXT},$<)) $@; rm $(subst ${SRCDIR},,$(subst .c,${CPPEXT},$^))
CFLAGS=${TMPCFLAGS} /Fo$(subst ${HOSTTMPDIR},${TMPDIR},$@) $(subst ${HOSTTMPDIR},${TMPDIR},$<)
CFLAGSPP=${TMPCFLAGS} /Fo$(subst ${HOSTTMPDIR},${TMPDIR},$@) /Tc$(subst ${HOSTTMPDIR},${TMPDIR},$<)
LDFLAGS=/nologo /out:$@ $^
TESTO:=
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
PROXIFIEDO:=
LD=${CC}
endif
endif
endif

TESTSUITES=$(subst .c,,$(wildcard ${HOSTTSTDIR}*_test.c))
DATS=$(subst ${HOSTTSTDIR},${HOSTTMPDIR},$(subst _test,_data.h,${TESTSUITES}))

info:
	echo ${DATS}
	echo ${CC}
#
# Produce a pre-processed file for the code to test
#
.PRECIOUS: ${HOSTTMPDIR}%${CPPEXT}
${HOSTTMPDIR}%${CPPEXT}: ${HOSTSRCDIR}%.c
	${Q}${CC} ${PPFLAGS}

.PRECIOUS: ${HOSTTMPDIR}%.sym
${HOSTTMPDIR}%.sym: ${HOSTTMPDIR}%${CPPEXT}
	${Q}${TCG}

.NOTPARALELL: ${HOSTTMPDIR}%_data.h
.PRECIOUS: ${HOSTTMPDIR}%_data.h
${HOSTTMPDIR}%_data.h: ${HOSTTMPDIR}%.sym ${HOSTTSTDIR}%_test.c
	${Q}${TSG}

.PRECIOUS: ${HOSTTMPDIR}%_mocks.c
${HOSTTMPDIR}%_mocks.c: ${HOSTTMPDIR}%.sym ${HOSTTMPDIR}%_data.h
	${Q}${TMG}

.PRECIOUS: ${HOSTTMPDIR}%_proxified${CPPEXT}
${HOSTTMPDIR}%_proxified${CPPEXT}: ${HOSTTMPDIR}%.sym ${HOSTTMPDIR}%${CPPEXT}
	${Q}${TAM}

.PRECIOIS: ${HOSTTMPDIR}%_runner.c
${HOSTTMPDIR}%_runner.c: ${HOSTTSTDIR}%_test.c ${HOSTTMPDIR}%_data.h
	${Q}${TTG}

#
# Compile the test suite to an object file
#
${HOSTTMPDIR}%.o: ${HOSTTMPDIR}%.c
	${Q}${CC} ${CFLAGS}

${HOSTTMPDIR}%.o: ${HOSTTMPDIR}%${CPPEXT}
	${Q}${CC} ${CFLAGSPP}

#${HOSTSRCDIR}%.o: ${HOSTTMPDIR}%.p
#	${Q}${CC} ${CFLAGSPP}

.PRECIOUS: ${HOSTSRCDIR}%_proxified.o
${HOSTSRCDIR}%_proxified.o: ${HOSTTMPDIR}%_proxified${CPPEXT} ${HOSTTMPDIR}%_data.h
	${Q}${CC} ${CFLAGSPP}; ${PROXIFIEDO}

.PRECIOUS: ${HOSTTMPDIR}%_mocks.o
${HOSTTMPDIR}%_mocks.o: ${HOSTTMPDIR}%_mocks.c

.PRECIOUS: ${HOSTTMPDIR}%_runner.o
${HOSTTMPDIR}%_runner.o: ${HOSTTMPDIR}%_runner.c

.NOTPARALELL: ${HOSTTMPDIR}%_test.o
.PRECIOUS: ${HOSTTMPDIR}%_test.o
${HOSTTMPDIR}%_test.o: ${HOSTTSTDIR}%_test.c ${HOSTTMPDIR}%_data.h
	${Q}${CC} ${CFLAGS}; ${TESTO}

#
# Link the test suite object to the test case object
#
# TODO: Make tarsio.o a shared library instead
#
%_test${EXE}: ${HOSTTMPDIR}%_test.o ${HOSTSRCDIR}%_proxified.o ${HOSTTMPDIR}%_runner.o ${HOSTTMPDIR}%_mocks.o
	${LD} ${LDFLAGS}

.PHONY: clean
clean::
	${Q}${RM} *~ ${HOSTTMPDIR}*.sym ${HOSTTMPDIR}*_data.h ${HOSTTMPDIR}*_data.o ${HOSTTMPDIR}*.p ${HOSTTMPDIR}*.pp ${HOSTTMPDIR}*.i ${HOSTTMPDIR}*_proxified* ${HOSTSRCDIR}*_proxified* ${HOSTTMPDIR}*_runner* ${HOSTTMPDIR}*_mocks* ${HOSTTMPDIR}*_test* *_test *.o ${HOSTTMPDIR}file*.asm ${HOSTTMPDIR}file*.o *.i

check:: ${DATS}
