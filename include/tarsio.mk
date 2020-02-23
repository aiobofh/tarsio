#
# Generic GNU Make include file for automatic builds of all Tarsio artifacts.
#
# Using this file
# ---------------
#
# Include it
# ^^^^^^^^^^
#
# To include this file in a Makefile of a project use the include directive
# of GNU Make. This way of including (and this file istelf) rely on a correct
# installation of Tarsio where it can be located using pkg-config.
#
# Example::
#
# -include $(shell pkg-config --variable=includedir tarsio)/tarsio.mk
#
# Control it
# ^^^^^^^^^^
#
# There are several more or less useful features that can be controlled from
# a software projects Makefile, to make the most out of Tarsio.
#
# Out-of-source-build
# ~~~~~~~~~~~~~~~~~~~
#
# Many developers prefer their binaries to be built in another folder than
# the one where the source-code resides. This is controlled by setting the
# TBUILDROOT variable in the Makefile that includes tarsio.mk. Binary
# object files can also be separated to be stored in some specific location.
# This is controlled by setting the TOBJROOT variable in the Makefile
# that includes tarsio.mk
#
# Example::
#
# TBUILDROOT=../build
# TOBJROOT=../objs
#
# Out-of-source-checks
# ~~~~~~~~~~~~~~~~~~~~
#
# The same as out-of-source-builds where many developers thinks it's more
# aesthetically pleasing to have a check/test-folder where all the checks are
# stored, instead of having them in the same directory as the actual source
# code. This is controlled by setting the TTESTROOT variable in the
# Makefile that includes tarsio.mk. If the check-folder has its own Makefile
# to build and run checks the path to the source files must be set too. This
# is controlled by the TSRCROOT variable in the Makefile that includes
# tarsio.mk
#
# Example::
#
# TOTESTROOT=../test
# TSRCROOT=../src
#
# Generated files for inclusion in checks
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
# There are files that need to be included in any check-suite. For example
# the *_data.h file which contains declarations of things that are essential
# for the check-suite.
#
# Example::
#
# TINCROOT=../test
#
# Faster builds with RAM-disk for temporary files
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
# It's sometimes useful to keep all temporary files (generated by Tarsio) in
# super-fast location, like a RAM-disk. This is controlled by setting the
# TTMPROOT varialbe in the Makefile that includes tarsio.mk
#
# Example::
#
# TTMPROOT=/tmp/$USER/my_project
#
# If you don't like the default _check.c suffix
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Some people do not like the "check" concept as replacement for "test", hence
# there is a setting for this as well. This is controlled by setting the
# TCHECKSUFFIX variable in the Makefile that includes tarsio.mk. Since tarsio.mk
# tries to keep a 1:1 filename mapping between design and checks this is a bit
# automagic.
#
# Example::
#
# TCHECKSUFFIX:=_test
#
# This will be inserted into the file name when the build system is looking for
# files. For example foo.c will be checked with foo_test.c if the variable is set
# to ``_test``.
#

Q?=@

MSG:="was not found in your PATH, please install Tarsio (https://github.com/aiobofh/tarsio)"

TCG:=$(if $(shell PATH=$(PATH) which tcg),$(shell which tcg),$(error "tcg ${MSG}"))
TSG:=$(if $(shell PATH=$(PATH) which tsg),$(shell which tsg),$(error "tsg ${MSG}"))
TMG:=$(if $(shell PATH=$(PATH) which tmg),$(shell which tmg),$(error "tmg ${MSG}"))
TAM:=$(if $(shell PATH=$(PATH) which tam),$(shell which tam),$(error "tam ${MSG}"))
TTG:=$(if $(shell PATH=$(PATH) which ttg),$(shell which ttg),$(error "ttg ${MSG}"))

TARSIOINCDIR:=$(shell pkg-config --variable=includedir tarsio)

TARSIOCFLAGS:=$(shell pkg-config --cflags tarsio) -I.

ifdef TSRCROOT
	TARSIOCFLAGS+=-I${TSRCROOT}
endif

TSTDIR=${TTESTROOT}
TMPDIR=${TTMPROOT}
SRCDIR=${TSRCROOT}

TCHECKSUFFIX?=_check

all: check

.PHONY: check
check:: $(subst .c,,$(wildcard *${TCHECKSUFFIX}.c))
	${Q}for i in $^; do ./$$i -c; done; echo ""

.PHONY: xml
xml:: $(subst .c,.xml,$(wildcard *${TCHECKSUFFIX}.c))

${TTMPROOT}.placeholder:
	${Q}mkdir -p $(dir $@) && touch $@


.PRECIOUS: ${TBUILDROOT}%.xml
${TBUILDROOT}%.xml: %
	${Q}./$< -x > $@

.PHONY: tarsio_info
tarsio_info:
	@echo ""; \
	echo "Tarsio version v$(shell pkg-config --modversion tarsio) installed in $(shell pkg-config --variable=prefix tarsio)"; \
	echo ""; \
	echo " Tarsio C-flags:  ${TARSIOCFLAGS} (-I flags should be in your include path for IDEs for less warnings)"; \
	echo ""; \
	echo " TCHECKSUFFIX: ${TCHECKSUFFIX}"; \
	echo " TSRCROOT: ${TSRCROOT}"; \
	echo " TTESTROOT: ${TTESTROOT}"; \
	echo " TTMPROOT: ${TTMPROOT}"; \
	echo ""; \
	echo "Sources"; \
	echo "-------"; \
	echo ""; \
	for i in $$(ls -1 ${TSRCROOT}*.c | grep -v ${TCHECKSUFFIX}.c | grep -v '_mocks.c' | grep -v '_runner.c'); do \
	  echo -n " $$i"; \
	done; \
	echo ""; echo ""; \
	echo "Checks"; \
	echo "------"; \
	echo ""; \
	for i in $$(ls -1 ${TTESTROOT}*${TCHECKSUFFIX}.c); do \
	  echo -n " $$i"; \
	done; \
	echo ""; echo ""; \

.PRECIOUS: ${TTMPROOT}%.pp
${TTMPROOT}%.pp: ${TSRCROOT}%.c ${TTMPROOT}.placeholder
	${Q}${CC} -O0 ${CFLAGS} ${TARSIOCFLAGS} -Dmain=__tarsio_replace_main -E -c $< > $@

.PRECIOUS: ${TTMPROOT}%.sym
${TTMPROOT}%.sym: ${TTMPROOT}%.pp ${TTMPROOT}.placeholder
	${Q}${TCG} $< $@

.NOTPARALLEL: ${TINCROOT}%_data.h
.PRECIOUS: ${TINCROOT}%_data.h
${TINCROOT}%_data.h: ${TTMPROOT}%.sym ${TTESTROOT}%${TCHECKSUFFIX}.c
	${Q}${TSG} $^ > $@

.PRECIOUS: ${TTMPROOT}%_mocks.c
${TTMPROOT}%_mocks.c: ${TTMPROOT}%.sym ${TINCROOT}%_data.h ${TTMPROOT}.placeholder
	${Q}${TMG} $(filter-out ${TTMPROOT}.placeholder,$^) > $@

.PRECIOUS: ${TTMPROOT}%_proxified.pp
.NOTPARALLEL: ${TTMPROOT}%_proxified.pp
${TTMPROOT}%_proxified.pp: ${TTMPROOT}%.sym ${TTMPROOT}%.pp ${TTMPROOT}.placeholder
	${Q}${TAM} $(filter-out ${TTMPROOT}.placeholder,$^) > $@

.PRECIOUS: ${TTMPROOT}%_runner.c
${TTMPROOT}%_runner.c: ${TTESTROOT}%${TCHECKSUFFIX}.c ${TINCROOT}%_data.h ${TTMPROOT}.placeholder
	${Q}${TTG} $(filter-out ${TTMPROOT}.placeholder,$^) > $@

.PRECIOUS: ${TOBJROOT}%_proxified.o
${TOBJROOT}%_proxified.o: ${TTMPROOT}%_proxified.pp
	${Q}${CC} ${CFLAGS} ${TARSIOCFLAGS} -x cpp-output -c $< -o $@

.PRECIOUS: ${TERSIOOBJROOT}%_mocks.o
${TOBJROOT}%_mocks.o: ${TTMPROOT}%_mocks.c
	${Q}${CC} ${CFLAGS} ${TARSIOCFLAGS} ${RUNNERCFLAGS} -o $@ -c $<

.PRECIOUS: ${TOBJROOT}%_runner.o
${TOBJROOT}%_runner.o: ${TTMPROOT}%_runner.c
	${Q}${CC} ${CFLAGS} ${TARSIOCFLAGS} ${RUNNERCFLAGS} -o $@ -c $<

.NOTPARALLEL: ${TOBJROOT}%${TCHECKSUFFIX}.o
.PRECIOUS: ${TOBJROOT}%${TCHECKSUFFIX}.o
${TOBJROOT}%${TCHECKSUFFIX}.o: ${TTESTROOT}%${TCHECKSUFFIX}.c ${TINCROOT}%_data.h
	${Q}${CC} ${CFLAGS} -Dmain=__tarsio_replace_main ${TARSIOCFLAGS} -o $@ -c $<

.PRECIOIS: ${TOBJROOT}tarsio.o
${TOBJROOT}tarsio.o: ${TARSIOINCDIR}/tarsio.c
	${Q}${CC} ${CFLAGS} ${TARSIOCFLAGS} ${RUNNERCFLAGS} -o $@ -c $<

.PRECIOUS: ${TBUILDROOT}%${TCHECKSUFFIX}
${TBUILDROOT}%${TCHECKSUFFIX}: ${TOBJROOT}%_proxified.o ${TOBJROOT}%_runner.o ${TOBJROOT}%_mocks.o ${TOBJPROOT}%${TCHECKSUFFIX}.o ${TOBJROOT}tarsio.o
	${Q}${CC} ${LDFLAGS} -o $@ $^

clean::
	${Q}rm -f ${TTMPROOT}*.pp ${TTMPROOT}*.sym ${TINCROOT}*_data.h ${TTMPROOT}*_mocks.c ${TTMPROOT}*_proxified.c ${TTMPROOT}*_runner.c ${TBOJROOT}*.o ${TBUILDROOT}*${TCHECKSUFFIX} ${TBUILDROOT}*${TCHECKSUFFIX}.xml *~
