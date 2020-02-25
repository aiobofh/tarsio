#
# Makefile for the Tarsio C Unit-testing framework
#
#               _______          _____ ___        ______
#                  |      ||    |         |    | |      |
#                  |      ||    |         |    | |      |
#                  |   ___||___ |         |___ | |______|
#
#               Copyright (C) 2020 AiO Secure Teletronics
#
# This is the make-file to build the binary tools in the Tarsio tool-chain. It
# might look a bit awful. But if you have an environment where you have wrapper
# scripts in your path for SAS/C, VBCC and VisualC it will make more sense.
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

NODEBUG?=-DNODEBUG
Q:=@
VERSION?=x.x.x
AUTHOR?=unknown

ifdef SASC
EXE:=
CPU:=68000
INCDIR:=/inc/
CFLAGS=CPU=${CPU} define NODEBUG define SASC define optimize noicons noversion includedirectory $(INCDIR) $<
CC:=sc
LDFLAGS=noicons noversion $^ link to $@
LD:=${CC}
else
ifdef VBCC
EXE:=
CPU:=68000
INCDIR:=../inc/
CFLAGS=${NODEBUG} -DVBCC=1 -O2 -c99 -c $^ -I${INCDIR} -o $@ -cpu=${CPU}
LDFLAGS=-O2 -c99 $^ -o $@
CC:=vc
LD:=${CC}
else
ifdef VC
EXE=.exe
INCDIR:=../inc/
CFLAGS=/DNODEBUG /D_CRT_SECURE_NO_DEPRECATE /nologo /c /O1 /W3 /MT /I${INCDIR} /Fo$@ $<
LDFLAGS=/nologo /out:$@ $^
CC=cl
LD:=link
else
EXE:=
INCDIR:=../inc/
ifdef SANITIZE
        SANITIZER_CFLAGS=-fsanitize=address,leak,undefined -fno-omit-frame-pointer
	SANITIZER_LDFLAGS=-lasan -fsanitize=address,leak,undefined
        ASAN_OPTIONS=detect_leaks=1
        export ASAN_OPTIONS
endif
CFLAGS=${NODEBUG} -Wall -fstack-protector-all -D_FORTIFY_SOURCE=2 -fPIE ${SANITIZER_CFLAGS} -Wmissing-prototypes -Wmissing-declarations -Wstrict-prototypes -Wpointer-arith -Wwrite-strings -Wcast-qual -Wcast-align -Wbad-function-cast -Wformat-security -Winline -std=c11 -pedantic -O2 -g -I${INCDIR} -o $@ -c $<
LDFLAGS=-flto -Wl,-z,nodlopen -Wl,-z,nodump -Wl,-z,noexecstack -pie -Wl,-z,relro -Wl,-z,now ${SANITIZER_LDFLAGS} -o $@ $^
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

sanitize: all

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

ttg${EXE}: ttg.o str.o file.o checkcase.o

.PHONY: clean
clean::
	$(Q)${RM} -f *~ *.o *.so *.library *.lnk *.p *.ueam version.h gts tsg tcg tmg tam ttg *.exe
