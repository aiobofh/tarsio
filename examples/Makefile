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

export PATH:=../src:${PATH}

all: write_file_check
	./write_file_check

# Generate pre-processed C-file for parsing by the Tarsio tools
%.pp: %.c
	gcc -O0 -Wall -std=c11 -pedantic -g -I. -I../include -Dmain=__tarsio_replace_main -E -c $^ > $@

# Generate a Symbol-cache file to be used by the other Tarsio tools
%.sym: %.pp
	tcg $^ $@

# Generate a header-file with all the mock storage structs
%_data.h: %.sym %_check.c
	tsg $^ > $@

# Generate mockup-function for the example project
%_mocks.c: %.sym %_data.h
	tmg $^ > $@

# Modify the pre-processed C source to call mock-up functions
%_proxified.pp: %.sym %.pp
	tam $^ > $@

# Generate the check-runner program
%_runner.c: %_check.c %_data.h
	ttg $^ > $@

# Compile the proxified object fil (it's a bit special)
%_proxified.o: %_proxified.pp
	gcc -Wall -std=c11 -pedantic -g -I. -I../include -x cpp-output -c -Wunused-function $< -o $@

# Generic C-source to object file target
%.o: %.c
	gcc -O0 -Wall -std=c11 -pedantic -g -I. -I../include -o $@ -c $<

# Link all the objects to an executable program with all the tests.
# Typically; this is where you need to add more dependencies if you
# want to call original code from checks.
%_check: %_proxified.o %_runner.o %_mocks.o %_check.o
	gcc -o $@ $^

# Cleanup all.
clean:
	rm -f *.pp *.sym *_data.h *_mocks.c *_proxified.c *_runner.c *.o *_check *~
