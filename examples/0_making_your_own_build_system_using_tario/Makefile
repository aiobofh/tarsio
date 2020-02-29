#
# Makefile for Tarsio example project
#
#               _______          _____ ___        ______
#                  |      ||    |         |    | |      |
#                  |      ||    |         |    | |      |
#                  |   ___||___ |         |___ | |______|
#
#               Copyright (C) 2020 AiO Secure Teletronics
#
# This Makefile will compile (and run) the example project for Tarsio.
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

MAKEFLAGS += --no-builtin-rules
.SUFFIXES:

Q?=@

ifdef Q
	HINT=
else
	HINT=; echo; echo "All the files are kept in this folder, take a looksie to get a feel for what Tarsio creates for you"; echo
endif

#
# Normally this should be:
#
# TARSIOINCDIR:=$(shell pkg-config --variable=includedir tarsio)
# TARSIOC:=${TARSIOINCDIR)tarsio.c
#
# But since this example should build and run without having to install Tarsio on
# the system, let's just traverse the directory tree relative.
#
TARSIOINCDIR:=../../include/tarsio
TARSIOC=../../src/tarsio.c

all: write_file
	${Q}echo "Built the write_file program, if you intended to run the" && \
	echo "checks just type 'make check' instead."

check: write_file_data.h write_file_check
	${Q}./write_file_check ${HINT}

write_file: write_file.c
	${Q}gcc -O2 -Wall -std=c11 -pedantic -o $@ $^

# Generate pre-processed C-file for parsing by the Tarsio tools
.PRECIOUS: %.pp
%.pp: %.c
	${Q}gcc -O0 -Wall -std=c11 -pedantic -g -I. -I${TARSIOINCDIR} -Dmain=__tarsio_replace_main -E -c $^ > $@

# Generate a Symbol-cache file to be used by the other Tarsio tools
.PRECIOUS: %.sym
%.sym: %.pp
	${Q}tcg $^ $@

# Generate a header-file with all the mock storage structs
.PRECIOUS: %_data.h
%_data.h: %.sym %_check.c
	${Q}tsg $^ > $@

# Generate mockup-function for the example project
.PRECIOUS: %_mocks.c
%_mocks.c: %.sym %_data.h
	${Q}tmg $^ > $@

# Modify the pre-processed C source to call mock-up functions
.PRECIOUS: %_proxified.pp
%_proxified.pp: %.sym %.pp %_data.h
	${Q}tam $^ > $@

# Generate the check-runner program
.PRECIOUS: %_runner.c
%_runner.c: %_check.c %_data.h
	${Q}ttg $^ > $@

# Compile the proxified object fil (it's a bit special)
.PRECIOUS: %_proxified.o
%_proxified.o: %_proxified.pp
	${Q}gcc -Wall -std=c11 -pedantic -g -I. -I${TARSIOINCDIR} -x cpp-output -c -Wunused-function $< -o $@

# Generic C-source to object file target
.PRECIOUS: %.o
%.o: %.c
	${Q}gcc -O0 -Wall -std=c11 -pedantic -g -I. -I${TARSIOINCDIR} -o $@ -c $<

# Re-use a lot of check-runner code between check-suites by just linking an
# already compiled runtime of the Tarsio API.
tarsio.o: ${TARSIOC}
	${Q}gcc -O0 -Wall -std=c11 -pedantic -g -I. -I${TARSIOINCDIR} -o $@ -c $<

# Link all the objects to an executable program with all the tests.
# Typically; this is where you need to add more dependencies if you
# want to call original code from checks.
.PRECIOUS: %_check
%_check: %_proxified.o %_runner.o %_mocks.o %_check.o tarsio.o %_data.h
	${Q}gcc -o $@ $(filter-out $(lastword $^),$^)

# Cleanup all.
clean:
	${Q}rm -f *.pp *.sym *_data.h *_mocks.c *_proxified.c *_runner.c *.o *_check *~ tarsio.o write_file
