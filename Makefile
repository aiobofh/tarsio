#
# Makefile for Tarsio
#
#               _______          _____ ___        ______
#                  |      ||    |         |    | |      |
#                  |      ||    |         |    | |      |
#                  |   ___||___ |         |___ | |______|
#
#               Copyright (C) 2020 AiO Secure Teletronics
#
# This is the main mail-file for building distributions and running all the
# regression checks of the Tarso project
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

PREFIX?=/usr/local
TARSIOHOME?=$(realpath .)

MAKEFLAGS += --no-builtin-rules
.SUFFIXES:

Q=@

export VERSION_MAJOR:=1
export VERSION_MINOR:=0
export VERSION_PATCH:=0
export AUTHOR:=Joakim Ekblad
export VERSION:=${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}
export Q

PKG_CONFIG=$(shell which pkg-config)
ifneq (,${PKG_CONFIG})
	INSTALL_PKG_CONFIG=install tarsio.pc /usr/share/pkgconfig/tarsio.pc
	REMOVE_PKG_CONFIG=/usr/share/pkgconfig/tarsio.pc
else
	INSTALL_PKG_CONFIG=echo "Warning: no pkg-config, skipping installation of tarsio.pc file" >&2
endif

BINTARGETS=tam tcg tmg tsg ttg
INCTARGETS=tarsio.mk coverage.mk tarsio.c tarsio.h
RST2MAN=$(shell which rst2man)

ifneq (,${RST2MAN})
	MANTARGETS=$(addsuffix .3,${BINTARGETS})
endif

all: local_install

info:
	${Q}echo ${VERSION} && \
	echo ${PKG_CONFIG} && \
	echo ${INSTALL_PKG_CONFIG} && \
	echo ${REMOVE_PKG_CONFIG} && \
	$(MAKE) --no-print-directory -C src info

.NOTPARALLEL: check-all
.PHONY: check-all
check-all: clean
	${Q}echo $@ && \
	echo "----------------------------------------------------------" && \
	echo "Native:" && \
	$(MAKE) --no-print-directory -C src && \
	$(MAKE) --no-print-directory -C test && \
	${MAKE} --no-print-directory -C examples check && \
	${MAKE} --no-print-directory -C examples clean && \
	$(MAKE) --no-print-directory -C src clean && \
	$(MAKE) --no-print-directory -C test clean && \
	echo "SAS/C using Vamos:" && \
	$(MAKE) --no-print-directory -C src SASC=1 && \
	$(MAKE) --no-print-directory -C test SASC=1 && \
	$(MAKE) --no-print-directory -C src clean && \
	$(MAKE) --no-print-directory -C test clean && \
	echo "VBCC cross compile, excution using Vamos:" && \
	$(MAKE) --no-print-directory -C src VBCC=1 && \
	$(MAKE) --no-print-directory -C test VBCC=1 && \
	$(MAKE) --no-print-directory -C src clean && \
	$(MAKE) --no-print-directory -C test clean # && \ VisualC not working yet
#	echo "VC cross compile, excution using Wine:" && \
#	$(MAKE) --no-print-directory -C src VC=1 && \
#	$(MAKE) --no-print-directory -C test VC=1 && \
#	$(MAKE) --no-print-directory -C src clean && \
#	$(MAKE) --no-print-directory -C test clean

check-sasc: clean
	$(MAKE) --no-print-directory -C src SASC=1 && \
	$(MAKE) --no-print-directory -C test SASC=1

.NOTPARALLEL: check
.PHONY: check
check:
	${Q}${MAKE} --no-print-directory -C test && \
	${MAKE} --no-print-directory -C test clean && \
	${MAKE} clean Q=${Q}

tarsio.pc:
	${Q}echo "Name: tarsio" > $@; \
	echo "Version: ${VERSION}" >> $@; \
	echo "Description: Tarsio - A minimalistic automocking unit-checking/testing framework" >> $@; \
	echo "prefix=${PREFIX}" >> $@; \
	echo "includedir=${PREFIX}/include/tarsio" >> $@; \
	echo "srcdir=${PREFIX}/include/tarsio" >> $@; \
	echo "bindir=${PREFIX}/bin" >> $@; \
	echo "Cflags: -I${PREFIX}/include/tarsio" >> $@

.PHONY: man-pages
man-pages:
	${Q}${MAKE} --no-print-directory -C doc

src/tam:
	${Q}${MAKE} --no-print-directory -C src tam

src/tcg:
	${Q}${MAKE} --no-print-directory -C src tcg

src/tmg:
	${Q}${MAKE} --no-print-directory -C src tmg

src/tsg:
	${Q}${MAKE} --no-print-directory -C src tsg

src/ttg:
	${Q}${MAKE} --no-print-directory -C src ttg

doc/tam.3:
	${Q}${MAKE} --no-print-directory -C doc tam.3

doc/tcg.3:
	${Q}${MAKE} --no-print-directory -C doc tcg.3

doc/tmg.3:
	${Q}${MAKE} --no-print-directory -C doc tmg.3

doc/tsg.3:
	${Q}${MAKE} --no-print-directory -C doc tsg.3

doc/ttg.3:
	${Q}${MAKE} --no-print-directory -C doc ttg.3

bin/.placeholder:
	${Q}mkdir -p $(dir $@) && touch $@

bin/%: src/% bin/.placeholder
	${Q}cp $< $@

include/tarsio/.placeholder:
	${Q}mkdir -p $(dir $@) && touch $@

include/tarsio/%: inc/% include/tarsio/.placeholder
	${Q}cp $< $@

include/tarsio/tarsio.c: src/tarsio.c include/tarsio/.placeholder
	${Q}cp $< $@

man/man3/.placeholder:
	${Q}mkdir -p $(dir $@) && touch $@

man/man3/%.3: doc/%.3 man/man3/.placeholder
	${Q}cp $< $@

.PHONY: local_intall
local_install: $(addprefix bin/,${BINTARGETS}) $(addprefix include/tarsio/,${INCTARGETS}) $(addprefix man/man3/,${MANTARGETS}) tarsio.pc

#	git status | grep 'git addd' >/dev/null && ((echo "WARNING: There are untracked files, investigate with 'git status'." >&2 && false) || (true)) && \
#	git status | grep 'git reset' >/dev/null && ((echo "WARNING: There are uncommitted changes, investigate with 'git status'." >&2 && false) || (true)) && \

.NOTPARALLEL: tarsio-${VERSION}.tar.gz
tarsio-${VERSION}.tar.gz: check-all
	${Q}echo "" && \
	echo $@ && \
	echo "----------------------------------------------------------" && \
	${MAKE} --no-print-directory -C src clean && \
	${MAKE} --no-print-directory -C test clean && \
	mkdir -p tarsio-${VERSION} && \
	cp -r README.rst Makefile src inc doc test examples tarsio-${VERSION}/. && \
	tar -c tarsio-${VERSION} | gzip > $@ && \
	rm -rf tarsio-${VERSION}

.NOTPARALLEL: tarsio-${VERSION}-src.lha
tarsio-${VERSION}-src.lha: check-all
	${Q}echo "" && \
	echo $@ && \
	echo "----------------------------------------------------------" && \
	${MAKE} --no-print-directory -C src clean && \
	${MAKE} --no-print-directory -C test clean && \
	mkdir -p tarsio-${VERSION} && \
	mkdir -p tarsio-${VERSION}/src && \
	cp -r README.rst smakefile src inc doc test examples tarsio-${VERSION}/src/. && \
	echo "#define VERSION \"${VERSION}\"" > tarsio-${VERSION}/src/src/version.h && \
	echo "#define AUTHOR \"${AUTHOR}\"" >> tarsio-${VERSION}/src/src/version.h && \
	jlha -cq $@ tarsio-${VERSION} && \
	rm -rf tarsio-${VERSION}

.NOTPARALLEL: tarsio-${VERSION}-sasc-68000-bin.lha
tarsio-${VERSION}-sasc-68000-bin.lha: check-all
	${Q}echo "" && \
	echo $@ && \
	echo "----------------------------------------------------------" && \
	${MAKE} --no-print-directory -C src clean && \
	${MAKE} --no-print-directory -C test clean && \
	mkdir -p tarsio-${VERSION} && \
	mkdir -p tarsio-${VERSION}/c && \
	mkdir -p tarsio-${VERSION}/libs && \
	mkdir -p tarsio-${VERSION}/include && \
	cp -r README.rst Makefile doc tarsio-${VERSION}/. && \
	cp inc/tarsio.smk inc/tarsio.h src/tarsio.c tarsio-${VERSION}/include/. && \
	${MAKE} --no-print-directory -C src SASC=1 CPU=68000 && \
	cp src/tcg src/tsg src/tmg src/tam src/ttg tarsio-${VERSION}/c/. && \
	jlha -cq $@ tarsio-${VERSION} && \
	rm -rf tarsio-${VERSION}

.NOTPARALLEL: tarsio-${VERSION}-sasc-68030-bin.lha
tarsio-${VERSION}-sasc-68030-bin.lha: check-all
	${Q}echo "" && \
	echo $@ && \
	echo "----------------------------------------------------------" && \
	${MAKE} --no-print-directory -C src clean && \
	${MAKE} --no-print-directory -C test clean && \
	mkdir -p tarsio-${VERSION} && \
	mkdir -p tarsio-${VERSION}/c && \
	mkdir -p tarsio-${VERSION}/libs && \
	mkdir -p tarsio-${VERSION}/include && \
	cp -r README.rst Makefile doc tarsio-${VERSION}/. && \
	cp inc/tarsio.smk inc/tarsio.h src/tarsio.c tarsio-${VERSION}/include/. && \
	${MAKE} --no-print-directory -C src SASC=1 CPU=68030 && \
	cp src/tcg src/tsg src/tmg src/tam src/ttg tarsio-${VERSION}/c/. && \
	jlha -cq $@ tarsio-${VERSION} && \
	rm -rf tarsio-${VERSION}

.NOTPARALLEL: tarsio-${VERSION}-sasc-68020-bin.lha
tarsio-${VERSION}-sasc-68020-bin.lha: check-all
	${Q}echo "" && \
	echo $@ && \
	echo "----------------------------------------------------------" && \
	${MAKE} --no-print-directory -C src clean && \
	${MAKE} --no-print-directory -C test clean && \
	mkdir -p tarsio-${VERSION} && \
	mkdir -p tarsio-${VERSION}/c && \
	mkdir -p tarsio-${VERSION}/libs && \
	mkdir -p tarsio-${VERSION}/include && \
	cp -r README.rst Makefile doc tarsio-${VERSION}/. && \
	cp inc/tarsio.smk inc/tarsio.h src/tarsio.c tarsio-${VERSION}/include/. && \
	${MAKE} --no-print-directory -C src SASC=1 CPU=68020 && \
	cp src/tcg src/tsg src/tmg src/tam src/ttg tarsio-${VERSION}/c/. && \
	jlha -cq $@ tarsio-${VERSION} && \
	rm -rf tarsio-${VERSION}

.NOTPARALLEL: tarsio-${VERSION}-sasc-68040-bin.lha
tarsio-${VERSION}-sasc-68040-bin.lha: check-all
	${Q}echo "" && \
	echo $@ && \
	echo "----------------------------------------------------------" && \
	${MAKE} --no-print-directory -C src clean && \
	${MAKE} --no-print-directory -C test clean && \
	mkdir -p tarsio-${VERSION} && \
	mkdir -p tarsio-${VERSION}/c && \
	mkdir -p tarsio-${VERSION}/libs && \
	mkdir -p tarsio-${VERSION}/include && \
	cp -r README.rst Makefile doc tarsio-${VERSION}/. && \
	cp inc/tarsio.smk inc/tarsio.h src/tarsio.c tarsio-${VERSION}/include/. && \
	${MAKE} --no-print-directory -C src SASC=1 CPU=68040 && \
	cp src/tcg src/tsg src/tmg src/tam src/ttg tarsio-${VERSION}/c/. && \
	jlha -cq $@ tarsio-${VERSION} && \
	rm -rf tarsio-${VERSION}

.NOTPARALLEL: tarsio-${VERSION}-sasc-68060-bin.lha
tarsio-${VERSION}-sasc-68060-bin.lha: check-all
	${Q}echo "" && \
	echo $@ && \
	echo "----------------------------------------------------------" && \
	${MAKE} --no-print-directory -C src clean && \
	${MAKE} --no-print-directory -C test clean && \
	mkdir -p tarsio-${VERSION} && \
	mkdir -p tarsio-${VERSION}/c && \
	mkdir -p tarsio-${VERSION}/libs && \
	mkdir -p tarsio-${VERSION}/include && \
	cp -r README.rst Makefile doc tarsio-${VERSION}/. && \
	cp inc/tarsio.smk inc/tarsio.h src/tarsio.c tarsio-${VERSION}/include/. && \
	${MAKE} --no-print-directory -C src SASC=1 CPU=68060 && \
	cp src/tcg src/tsg src/tmg src/tam src/ttg tarsio-${VERSION}/c/. && \
	jlha -cq $@ tarsio-${VERSION} && \
	rm -rf tarsio-${VERSION}

.NOTPARALLEL: tarsio-${VERSION}-vbcc-68000-bin.lha
tarsio-${VERSION}-vbcc-68000-bin.lha: check-all
	${Q}echo "" && \
	echo $@ && \
	echo "----------------------------------------------------------" && \
	${MAKE} --no-print-directory -C src clean && \
	${MAKE} --no-print-directory -C test clean && \
	mkdir -p tarsio-${VERSION} && \
	mkdir -p tarsio-${VERSION}/c && \
	mkdir -p tarsio-${VERSION}/libs && \
	mkdir -p tarsio-${VERSION}/include && \
	cp -r README.rst Makefile doc tarsio-${VERSION}/. && \
	cp inc/tarsio.smk inc/tarsio.h src/tarsio.c tarsio-${VERSION}/include/. && \
	${MAKE} --no-print-directory -C src VBCC=1 && \
	cp src/tcg src/tsg src/tmg src/tam src/ttg tarsio-${VERSION}/c/. && \
	jlha -cq $@ tarsio-${VERSION} && \
	rm -rf tarsio-${VERSION}

.NOTPARALLEL: tarsio-${VERSION}-vbcc-68020-bin.lha
tarsio-${VERSION}-vbcc-68020-bin.lha: check-all
	${Q}echo "" && \
	echo $@ && \
	echo "----------------------------------------------------------" && \
	${MAKE} --no-print-directory -C src clean && \
	${MAKE} --no-print-directory -C test clean && \
	mkdir -p tarsio-${VERSION} && \
	mkdir -p tarsio-${VERSION}/c && \
	mkdir -p tarsio-${VERSION}/libs && \
	mkdir -p tarsio-${VERSION}/include && \
	cp -r README.rst Makefile inc doc tarsio-${VERSION}/. && \
	cp inc/tarsio.smk inc/tarsio.h src/tarsio.c tarsio-${VERSION}/include/. && \
	${MAKE} --no-print-directory -C src VBCC=1 CPU=68020 && \
	cp src/tcg src/tsg src/tmg src/tam src/ttg tarsio-${VERSION}/c/. && \
	jlha -cq $@ tarsio-${VERSION} && \
	rm -rf tarsio-${VERSION}

.NOTPARALLEL: tarsio-${VERSION}-vbcc-68030-bin.lha
tarsio-${VERSION}-vbcc-68030-bin.lha: check-all
	${Q}echo "" && \
	echo $@ && \
	echo "----------------------------------------------------------" && \
	${MAKE} --no-print-directory -C src clean && \
	${MAKE} --no-print-directory -C test clean && \
	mkdir -p tarsio-${VERSION} && \
	mkdir -p tarsio-${VERSION}/c && \
	mkdir -p tarsio-${VERSION}/libs && \
	mkdir -p tarsio-${VERSION}/include && \
	cp -r README.rst Makefile inc doc tarsio-${VERSION}/. && \
	cp inc/tarsio.smk inc/tarsio.h src/tarsio.c tarsio-${VERSION}/include/. && \
	${MAKE} --no-print-directory -C src VBCC=1 CPU=68030 && \
	cp src/tcg src/tsg src/tmg src/tam src/ttg tarsio-${VERSION}/c/. && \
	jlha -cq $@ tarsio-${VERSION} && \
	rm -rf tarsio-${VERSION}

.NOTPARALLEL: tarsio-${VERSION}-vbcc-68040-bin.lha
tarsio-${VERSION}-vbcc-68040-bin.lha: check-all
	${Q}echo "" && \
	echo $@ && \
	echo "----------------------------------------------------------" && \
	${MAKE} --no-print-directory -C src clean && \
	${MAKE} --no-print-directory -C test clean && \
	mkdir -p tarsio-${VERSION} && \
	mkdir -p tarsio-${VERSION}/c && \
	mkdir -p tarsio-${VERSION}/libs && \
	mkdir -p tarsio-${VERSION}/include && \
	cp -r README.rst Makefile inc doc tarsio-${VERSION}/. && \
	cp inc/tarsio.smk inc/tarsio.h src/tarsio.c tarsio-${VERSION}/include/. && \
	${MAKE} --no-print-directory -C src VBCC=1 CPU=68040 && \
	cp src/tcg src/tsg src/tmg src/tam src/ttg tarsio-${VERSION}/c/. && \
	jlha -cq $@ tarsio-${VERSION} && \
	rm -rf tarsio-${VERSION}

.NOTPARALLEL: tarsio-${VERSION}-vbcc-68060-bin.lha
tarsio-${VERSION}-vbcc-68060-bin.lha: check-all
	${Q}echo "" && \
	echo $@ && \
	echo "----------------------------------------------------------" && \
	${MAKE} --no-print-directory -C src clean && \
	${MAKE} --no-print-directory -C test clean && \
	mkdir -p tarsio-${VERSION} && \
	mkdir -p tarsio-${VERSION}/c && \
	mkdir -p tarsio-${VERSION}/libs && \
	mkdir -p tarsio-${VERSION}/include && \
	cp -r README.rst Makefile inc doc tarsio-${VERSION}/. && \
	cp inc/tarsio.smk inc/tarsio.h src/tarsio.c tarsio-${VERSION}/include/. && \
	${MAKE} --no-print-directory -C src VBCC=1 CPU=68060 && \
	cp src/tcg src/tsg src/tmg src/tam src/ttg tarsio-${VERSION}/c/. && \
	jlha -cq $@ tarsio-${VERSION} && \
	rm -rf tarsio-${VERSION}

.NOTPARALLEL: source-dist
source-dist: tarsio-${VERSION}.tar.gz tarsio-${VERSION}-src.lha
	${Q}echo "" && \
	echo $@ && \
	echo "----------------------------------------------------------" && \
	echo "Verifying tar.gz distribution:" && \
	tar xzf tarsio-${VERSION}.tar.gz && \
	${MAKE} --no-print-directory -C tarsio-${VERSION}/src check && \
	rm -rf tarsio-${VERSION} && \
	echo "Verifying lha distribution with SASC:" && \
	jlha -xq tarsio-${VERSION}-src.lha && \
	${MAKE} --no-print-directory -C tarsio-${VERSION}/src check SASC=1 && \
	rm -rf tarsio-${VERSION} && \
	echo "Verifying lha distribution with VBCC:" && \
	jlha -xq tarsio-${VERSION}-src.lha && \
	${MAKE} --no-print-directory -C tarsio-${VERSION}/src check VBCC=1 && \
	rm -rf tarsio-${VERSiON}

.NOTPARALLEL: bin-dist
bin-dist: tarsio-${VERSION}-sasc-68000-bin.lha tarsio-${VERSION}-sasc-68020-bin.lha tarsio-${VERSION}-sasc-68030-bin.lha tarsio-${VERSION}-sasc-68040-bin.lha tarsio-${VERSION}-sasc-68060-bin.lha tarsio-${VERSION}-vbcc-68000-bin.lha tarsio-${VERSION}-vbcc-68020-bin.lha tarsio-${VERSION}-vbcc-68030-bin.lha tarsio-${VERSION}-vbcc-68040-bin.lha tarsio-${VERSION}-vbcc-68060-bin.lha

.NOTPARALLEL: dist
.PHONY: dist
dist: source-dist bin-dist

.PHONY: install
install:
	${Q}mkdir -p ${PREFIX}/bin && \
	install -s bin/tcg ${PREFIX}/bin/tcg && \
	install -s bin/tam ${PREFIX}/bin/tam && \
	install -s bin/tmg ${PREFIX}/bin/tmg && \
	install -s bin/tsg ${PREFIX}/bin/tsg && \
	install -s bin/ttg ${PREFIX}/bin/ttg && \
	mkdir -p ${PREFIX}/include/tarsio && \
	install include/tarsio/tarsio.mk ${PREFIX}/include/tarsio/tarsio.mk && \
	install include/tarsio/coverage.mk ${PREFIX}/include/tarsio/coverage.mk && \
	install include/tarsio/tarsio.h ${PREFIX}/include/tarsio/tarsio.h && \
	install include/tarsio/tarsio.c ${PREFIX}/include/tarsio/tarsio.c && \
	${INSTALL_PKG_CONFIG} && \
	mkdir -p ${PREFIX}/man/man3 && \
	if [ -f man/man3/tcg.3 ]; then install man/man3/tcg.3 ${PREFIX}/man/man3/tcg.3; fi && \
	if [ -f man/man3/tam.3 ]; then install man/man3/tam.3 ${PREFIX}/man/man3/tam.3; fi && \
	if [ -f man/man3/tmg.3 ]; then install man/man3/tmg.3 ${PREFIX}/man/man3/tmg.3; fi && \
	if [ -f man/man3/tsg.3 ]; then install man/man3/tsg.3 ${PREFIX}/man/man3/tsg.3; fi && \
	if [ -f man/man3/ttg.3 ]; then install man/man3/ttg.3 ${PREFIX}/man/man3/ttg.3; fi

uninstall:
	${Q}${RM} -rf ${PREFIX}/bin/tcg ${PREFIX}/bin/tam ${PREFIX}/bin/tmg ${PREFIX}/bin/tsg ${PREFIX}/bin/ttg ${PREFIX}/include/tarsio ${REMOVE_PKG_CONFIG} ${PREFIX}/man/man3/tcg.3 ${PREFIX}/man/man3/tam.3 ${PREFIX}/man/man3/tmg.3 ${PREFIX}/man/man3/tsg.3 ${PREFIX}/man/man3/ttg.3

.NOTPARALLEL: clean
.PHONY: clean
clean:
	${Q}$(MAKE) --no-print-directory -C src clean && \
	$(MAKE) --no-print-directory -C test clean && \
	$(MAKE) --no-print-directory -C examples clean Q=@ && \
	${MAKE} --no-print-directory -C doc clean && \
	$(RM) -rf *~ bin include man inc/*~ *.uaem tarsio-${VERSION}* tarsio*.lha tarsio.pc
