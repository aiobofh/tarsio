#
# Makefile for the Tarsio C Unit-testing framework
#

NODEBUG?=-DNODEBUG
Q:=@
VERSION?=x.x.x
AUTHOR?=unknown

ifdef SASC
EXE:=
CPU:=68000
INCDIR:=/include/
CFLAGS=CPU=${CPU} define NODEBUG define SASC define optimize noicons noversion includedirectory $(INCDIR) $<
CC:=sc
LDFLAGS=noicons noversion $^ link to $@
LD:=${CC}
else
ifdef VBCC
EXE:=
CPU:=68000
INCDIR:=../include/
CFLAGS=${NODEBUG} -DVBCC=1 -O2 -c99 -c $^ -I${INCDIR} -o $@ -cpu=${CPU}
#CFLAGS=-O2 -c99 -c $^ -I${INCDIR} -o $@ -cpu=68000
LDFLAGS=-O2 -c99 $^ -o $@
CC:=vc
LD:=${CC}
else
ifdef VC
EXE=.exe
INCDIR:=../include/
CFLAGS=/DNODEBUG /D_CRT_SECURE_NO_DEPRECATE /nologo /c /O1 /W3 /MT /I${INCDIR} /Fo$@ $<
#CFLAGS=/D_CRT_SECURE_NO_DEPRECATE /nologo /c /O1 /W3 /MT /I${INCDIR} /Fo$@ $<
LDFLAGS=/nologo /out:$@ $^
CC=cl
LD:=link
else
EXE:=
INCDIR:=../include/
CFLAGS=${NODEBUG} -Wall -std=c11 -pedantic -O2 -g -I${INCDIR} -o $@ -c $<
#CFLAGS=-Wall -std=c11 -pedantic -O2 -g -I${INCDIR} -o $@ -c $<
LDFLAGS=-flto -o $@ $^
LD:=${CC}
ifdef COV
CFLAGS+=-fprofile-arcs -ftest-coverage
LDFLAGS+=-fprofile-arcs -lgcov --coverage
endif
endif
endif
endif

TSTDIR:=../test/

.SUFFIXES:

all: tcg${EXE} tsg${EXE} tmg${EXE} tam${EXE} ttg${EXE}
	@: # Force GNU Make to be silent if nothing need to be done

info:
	${Q}echo ${VERSION}

check:
	$(Q)$(MAKE) && \
	$(MAKE) --no-print-directory -C ${TSTDIR}

version.h: Makefile
	${Q}echo "#define VERSION \"${VERSION}\"" > version.h && \
	echo "#define AUTHOR \"${AUTHOR}\"" >> version.h

%.o: %.c version.h
	$(Q)$(CC) ${CFLAGS}

%:
	$(Q)$(LD) ${LDFLAGS}


#
# Tarsio tool chain
#
tcg${EXE}: tcg.o list.o prototype.o file.o argument.o symbol_usage.o datatype.o symbol_cache.o

tsg${EXE}: tsg.o str.o list.o prototype.o cpp.o symbol_cache.o file.o argument.o symbol_usage.o datatype.o

tmg${EXE}: tmg.o list.o prototype.o symbol_cache.o file.o argument.o symbol_usage.o datatype.o

tam${EXE}: tam.o list.o prototype.o symbol_cache.o file.o argument.o symbol_usage.o datatype.o

ttg${EXE}: ttg.o str.o file.o testcase.o

clean::
	$(Q)${RM} *~ *.o *.so *.library *.lnk *.p *.ueam version.h gts tsg tcg tmg tam ttg *.exe
