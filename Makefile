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

MAKEFLAGS += --no-builtin-rules
.SUFFIXES:

export VERSION_MAJOR:=1
export VERSION_MINOR:=0
export VERSION_PATCH:=0
export AUTHOR:=Joakim Ekblad
export VERSION:=${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}
export Q:=@

all: build

info:
	@echo ${VERSION} && \
	$(MAKE) --no-print-directory -C src info

.NOTPARALLEL: check-all
.PHONY: check-all
check-all: clean
	@echo $@ && \
	echo "----------------------------------------------------------" && \
	echo "Native:" && \
	$(MAKE) --no-print-directory -C src && \
	$(MAKE) --no-print-directory -C test && \
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
	@${MAKE} --no-print-directory -C test && \
	${MAKE} --no-print-directory -C test clean && \
	${MAKE} --no-print-directory -C examples check Q=@ && \
	${MAKE} --no-print-directory -C examples clean Q=@ && \
	${MAKE} clean Q=@

tarsio.pc:
	@echo "Name: tarsio" > $@; \
	echo "Version: ${VERSION}" >> $@; \
	echo "Description: Tarsio - A minimalistic automocking unit-checking/testing framework" >> $@; \
	echo "prefix=${PREFIX}" >> $@; \
	echo "includedir=${PREFIX}/include/tarsio" >> $@; \
	echo "srcdir=${PREFIX}/include/tarsio" >> $@; \
	echo "bindir=${PREFIX}/bin" >> $@; \
	echo "Cflags: -I${PREFIX}/include/tarsio" >> $@

.PHONY: man-pages
man-pages:
	@${MAKE} --no-print-directory -C doc

.PHONY: build
build: tarsio.pc man-pages
	@${MAKE} --no-print-directory -C src

#	git status | grep 'git addd' >/dev/null && ((echo "WARNING: There are untracked files, investigate with 'git status'." >&2 && false) || (true)) && \
#	git status | grep 'git reset' >/dev/null && ((echo "WARNING: There are uncommitted changes, investigate with 'git status'." >&2 && false) || (true)) && \

.NOTPARALLEL: tarsio-${VERSION}.tar.gz
tarsio-${VERSION}.tar.gz: check-all
	@echo "" && \
	echo $@ && \
	echo "----------------------------------------------------------" && \
	${MAKE} --no-print-directory -C src clean && \
	${MAKE} --no-print-directory -C test clean && \
	mkdir -p tarsio-${VERSION} && \
	cp -r README.rst Makefile src include doc test tarsio-${VERSION}/. && \
	tar -c tarsio-${VERSION} | gzip > $@ && \
	rm -rf tarsio-${VERSION}

.NOTPARALLEL: tarsio-${VERSION}-src.lha
tarsio-${VERSION}-src.lha: check-all
	@echo "" && \
	echo $@ && \
	echo "----------------------------------------------------------" && \
	${MAKE} --no-print-directory -C src clean && \
	${MAKE} --no-print-directory -C test clean && \
	mkdir -p tarsio-${VERSION} && \
	mkdir -p tarsio-${VERSION}/src && \
	cp -r README.rst Makefile src include doc test tarsio-${VERSION}/src/. && \
	jlha -cq $@ tarsio-${VERSION} && \
	rm -rf tarsio-${VERSION}

.NOTPARALLEL: tarsio-${VERSION}-sasc-68000-bin.lha
tarsio-${VERSION}-sasc-68000-bin.lha: check-all
	@echo "" && \
	echo $@ && \
	echo "----------------------------------------------------------" && \
	${MAKE} --no-print-directory -C src clean && \
	${MAKE} --no-print-directory -C test clean && \
	mkdir -p tarsio-${VERSION} && \
	mkdir -p tarsio-${VERSION}/c && \
	mkdir -p tarsio-${VERSION}/libs && \
	cp -r README.rst Makefile include doc tarsio-${VERSION}/. && \
	${MAKE} --no-print-directory -C src SASC=1 && \
	cp src/tcg src/tsg src/tmg src/tam src/ttg tarsio-${VERSION}/c/. && \
	jlha -cq $@ tarsio-${VERSION} && \
	rm -rf tarsio-${VERSION}

.NOTPARALLEL: tarsio-${VERSION}-sasc-68020-bin.lha
tarsio-${VERSION}-sasc-68020-bin.lha: check-all
	@echo "" && \
	echo $@ && \
	echo "----------------------------------------------------------" && \
	${MAKE} --no-print-directory -C src clean && \
	${MAKE} --no-print-directory -C test clean && \
	mkdir -p tarsio-${VERSION} && \
	mkdir -p tarsio-${VERSION}/c && \
	mkdir -p tarsio-${VERSION}/libs && \
	cp -r README.rst Makefile include doc tarsio-${VERSION}/. && \
	${MAKE} --no-print-directory -C src SASC=1 CPU=68020 && \
	cp src/tcg src/tsg src/tmg src/tam src/ttg tarsio-${VERSION}/c/. && \
	jlha -cq $@ tarsio-${VERSION} && \
	rm -rf tarsio-${VERSION}

.NOTPARALLEL: tarsio-${VERSION}-sasc-68030-bin.lha
tarsio-${VERSION}-sasc-68030-bin.lha: check-all
	@echo "" && \
	echo $@ && \
	echo "----------------------------------------------------------" && \
	${MAKE} --no-print-directory -C src clean && \
	${MAKE} --no-print-directory -C test clean && \
	mkdir -p tarsio-${VERSION} && \
	mkdir -p tarsio-${VERSION}/c && \
	mkdir -p tarsio-${VERSION}/libs && \
	cp -r README.rst Makefile include doc tarsio-${VERSION}/. && \
	${MAKE} --no-print-directory -C src SASC=1 CPU=68030 && \
	cp src/tcg src/tsg src/tmg src/tam src/ttg tarsio-${VERSION}/c/. && \
	jlha -cq $@ tarsio-${VERSION} && \
	rm -rf tarsio-${VERSION}

.NOTPARALLEL: tarsio-${VERSION}-sasc-68040-bin.lha
tarsio-${VERSION}-sasc-68040-bin.lha: check-all
	@echo "" && \
	echo $@ && \
	echo "----------------------------------------------------------" && \
	${MAKE} --no-print-directory -C src clean && \
	${MAKE} --no-print-directory -C test clean && \
	mkdir -p tarsio-${VERSION} && \
	mkdir -p tarsio-${VERSION}/c && \
	mkdir -p tarsio-${VERSION}/libs && \
	cp -r README.rst Makefile include doc tarsio-${VERSION}/. && \
	${MAKE} --no-print-directory -C src SASC=1 CPU=68040 && \
	cp src/tcg src/tsg src/tmg src/tam src/ttg tarsio-${VERSION}/c/. && \
	jlha -cq $@ tarsio-${VERSION} && \
	rm -rf tarsio-${VERSION}

.NOTPARALLEL: tarsio-${VERSION}-sasc-68060-bin.lha
tarsio-${VERSION}-sasc-68060-bin.lha: check-all
	@echo "" && \
	echo $@ && \
	echo "----------------------------------------------------------" && \
	${MAKE} --no-print-directory -C src clean && \
	${MAKE} --no-print-directory -C test clean && \
	mkdir -p tarsio-${VERSION} && \
	mkdir -p tarsio-${VERSION}/c && \
	mkdir -p tarsio-${VERSION}/libs && \
	cp -r README.rst Makefile include doc tarsio-${VERSION}/. && \
	${MAKE} --no-print-directory -C src SASC=1 CPU=68060 && \
	cp src/tcg src/tsg src/tmg src/tam src/ttg tarsio-${VERSION}/c/. && \
	jlha -cq $@ tarsio-${VERSION} && \
	rm -rf tarsio-${VERSION}

.NOTPARALLEL: tarsio-${VERSION}-vbcc-68000-bin.lha
tarsio-${VERSION}-vbcc-68000-bin.lha: check-all
	@echo "" && \
	echo $@ && \
	echo "----------------------------------------------------------" && \
	${MAKE} --no-print-directory -C src clean && \
	${MAKE} --no-print-directory -C test clean && \
	mkdir -p tarsio-${VERSION} && \
	mkdir -p tarsio-${VERSION}/c && \
	mkdir -p tarsio-${VERSION}/libs && \
	cp -r README.rst Makefile include doc tarsio-${VERSION}/. && \
	${MAKE} --no-print-directory -C src VBCC=1 && \
	cp src/tcg src/tsg src/tmg src/tam src/ttg tarsio-${VERSION}/c/. && \
	jlha -cq $@ tarsio-${VERSION} && \
	rm -rf tarsio-${VERSION}

.NOTPARALLEL: tarsio-${VERSION}-vbcc-68020-bin.lha
tarsio-${VERSION}-vbcc-68020-bin.lha: check-all
	@echo "" && \
	echo $@ && \
	echo "----------------------------------------------------------" && \
	${MAKE} --no-print-directory -C src clean && \
	${MAKE} --no-print-directory -C test clean && \
	mkdir -p tarsio-${VERSION} && \
	mkdir -p tarsio-${VERSION}/c && \
	mkdir -p tarsio-${VERSION}/libs && \
	cp -r README.rst Makefile include doc tarsio-${VERSION}/. && \
	${MAKE} --no-print-directory -C src VBCC=1 CPU=68020 && \
	cp src/tcg src/tsg src/tmg src/tam src/ttg tarsio-${VERSION}/c/. && \
	jlha -cq $@ tarsio-${VERSION} && \
	rm -rf tarsio-${VERSION}

.NOTPARALLEL: tarsio-${VERSION}-vbcc-68030-bin.lha
tarsio-${VERSION}-vbcc-68030-bin.lha: check-all
	@echo "" && \
	echo $@ && \
	echo "----------------------------------------------------------" && \
	${MAKE} --no-print-directory -C src clean && \
	${MAKE} --no-print-directory -C test clean && \
	mkdir -p tarsio-${VERSION} && \
	mkdir -p tarsio-${VERSION}/c && \
	mkdir -p tarsio-${VERSION}/libs && \
	cp -r README.rst Makefile include doc tarsio-${VERSION}/. && \
	${MAKE} --no-print-directory -C src VBCC=1 CPU=68030 && \
	cp src/tcg src/tsg src/tmg src/tam src/ttg tarsio-${VERSION}/c/. && \
	jlha -cq $@ tarsio-${VERSION} && \
	rm -rf tarsio-${VERSION}

.NOTPARALLEL: tarsio-${VERSION}-vbcc-68040-bin.lha
tarsio-${VERSION}-vbcc-68040-bin.lha: check-all
	@echo "" && \
	echo $@ && \
	echo "----------------------------------------------------------" && \
	${MAKE} --no-print-directory -C src clean && \
	${MAKE} --no-print-directory -C test clean && \
	mkdir -p tarsio-${VERSION} && \
	mkdir -p tarsio-${VERSION}/c && \
	mkdir -p tarsio-${VERSION}/libs && \
	cp -r README.rst Makefile include doc tarsio-${VERSION}/. && \
	${MAKE} --no-print-directory -C src VBCC=1 CPU=68040 && \
	cp src/tcg src/tsg src/tmg src/tam src/ttg tarsio-${VERSION}/c/. && \
	jlha -cq $@ tarsio-${VERSION} && \
	rm -rf tarsio-${VERSION}

.NOTPARALLEL: tarsio-${VERSION}-vbcc-68060-bin.lha
tarsio-${VERSION}-vbcc-68060-bin.lha: check-all
	@echo "" && \
	echo $@ && \
	echo "----------------------------------------------------------" && \
	${MAKE} --no-print-directory -C src clean && \
	${MAKE} --no-print-directory -C test clean && \
	mkdir -p tarsio-${VERSION} && \
	mkdir -p tarsio-${VERSION}/c && \
	mkdir -p tarsio-${VERSION}/libs && \
	cp -r README.rst Makefile include doc tarsio-${VERSION}/. && \
	${MAKE} --no-print-directory -C src VBCC=1 CPU=68060 && \
	cp src/tcg src/tsg src/tmg src/tam src/ttg tarsio-${VERSION}/c/. && \
	jlha -cq $@ tarsio-${VERSION} && \
	rm -rf tarsio-${VERSION}

.NOTPARALLEL: source-dist
source-dist: tarsio-${VERSION}.tar.gz tarsio-${VERSION}-src.lha
	@echo "" && \
	echo $@ && \
	echo "----------------------------------------------------------" && \
	echo "Verifying tar.gz distribution:" && \
	tar xzf tarsio-${VERSION}.tar.gz && ${MAKE} --no-print-directory -C tarsio-${VERSION}/src check && rm -rf tarsio-${VERSION} && \
	echo "Verifying lha distribution with SASC:" && \
	jlha -xq tarsio-${VERSION}-src.lha && ${MAKE} --no-print-directory -C tarsio-${VERSION}/src check SASC=1 && rm -rf tarsio-${VERSION} && \
	echo "Verifying lha distribution with VBCC:" && \
	jlha -xq tarsio-${VERSION}-src.lha && ${MAKE} --no-print-directory -C tarsio-${VERSION}/src check VBCC=1 && rm -rf tarsio-${VERSiON}

.NOTPARALLEL: bin-dist
bin-dist: tarsio-${VERSION}-sasc-68000-bin.lha tarsio-${VERSION}-sasc-68020-bin.lha tarsio-${VERSION}-sasc-68030-bin.lha tarsio-${VERSION}-sasc-68040-bin.lha tarsio-${VERSION}-sasc-68060-bin.lha tarsio-${VERSION}-vbcc-68000-bin.lha tarsio-${VERSION}-vbcc-68020-bin.lha tarsio-${VERSION}-vbcc-68030-bin.lha tarsio-${VERSION}-vbcc-68040-bin.lha tarsio-${VERSION}-vbcc-68060-bin.lha

.NOTPARALLEL: dist
.PHONY: dist
dist: source-dist bin-dist

.PHONY: install
install: build
	@mkdir -p ${PREFIX}/bin && \
	install -s src/tcg ${PREFIX}/bin/tcg && \
	install -s src/tam ${PREFIX}/bin/tam && \
	install -s src/tmg ${PREFIX}/bin/tmg && \
	install -s src/tsg ${PREFIX}/bin/tsg && \
	install -s src/ttg ${PREFIX}/bin/ttg && \
	mkdir -p ${PREFIX}/include/tarsio && \
	install include/tarsio.mk ${PREFIX}/include/tarsio/tarsio.mk && \
	install include/coverage.mk ${PREFIX}/include/tarsio/coverage.mk && \
	install include/tarsio.h ${PREFIX}/include/tarsio/tarsio.h && \
	install src/tarsio.c ${PREFIX}/include/tarsio/tarsio.c && \
	install tarsio.pc /usr/share/pkgconfig/tarsio.pc && \
	mkdir -p ${PREFIX}/man/man3 && \
	install doc/tcg.3 ${PREFIX}/man/man3/tcg.3 && \
	install doc/tam.3 ${PREFIX}/man/man3/tam.3 && \
	install doc/tmg.3 ${PREFIX}/man/man3/tmg.3 && \
	install doc/tsg.3 ${PREFIX}/man/man3/tsg.3 && \
	install doc/ttg.3 ${PREFIX}/man/man3/ttg.3

uninstall:
	${RM}-rf ${PREFIX}/bin/tcg ${PREFIX}/bin/tam ${PREFIX}/bin/tmg ${PREFIX}/bin/tsg ${PREFIX}/bin/ttg ${PREFIX}/include/tarsio /usr/share/pkgconfig/tarsio.pc

.NOTPARALLEL: clean
.PHONY: clean
clean:
	@$(MAKE) --no-print-directory -C src clean && \
	$(MAKE) --no-print-directory -C test clean && \
	$(MAKE) --no-print-directory -C examples clean Q=@ && \
	${MAKE} --no-print-directory -C doc clean && \
	$(RM) -rf *~ include/*~ *.uaem tarsio-${VERSION}* tarsio*.lha tarsio.pc

