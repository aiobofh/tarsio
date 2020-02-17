#
# SMakefile to for building a complete check-suite using the Tarsio tool-chain
#
#               _______          _____ ___        ______
#                  |      ||    |         |    | |      |
#                  |      ||    |         |    | |      |
#                  |   ___||___ |         |___ | |______|
#
#               Copyright (C) 2020 AiO Secure Teletronics
#
# This is the make-file to build the binary tools in the Tarsio tool-chain using
# SAS/C.
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

SRCDIR=/src/
INCDIR=/include/
TSTDIR=/test/
COVDIR=T:
TMPDIR=T:

CFLAGS=define NODEBUG define SASC optimize noicons noversion includedirectory $(TMPDIR) includedirectory $(INCDIR) includedirectory $(SRCDIR)

$(TMPDIR)$(DUTNAME).p: $(SRCDIR)$(DUTNAME).c
	@$(CC) $(CFLASG) $(SRCDIR)$(DUTNAME).c preprocessoronly
	@copy $(SRCDIR)$(DUTNAME).p $(TMPDIR)$(DUTNAME).p
	@delete QUIET $(SRCDIR)$(DUTNAME).p

$(TMPDIR)$(DUTNAME).sym: $(TMPDIR)$(DUTNAME).p
	@$(SRCDIR)tcg $(TMPDIR)$(DUTNAME).p $(TMPDIR)$(DUTNAME).sym

$(TMPDIR)$(DUTNAME)_data.h: $(TMPDIR)$(DUTNAME).sym $(DUTNAME)_test.c
	@$(SRCDIR)tsg $(TMPDIR)$(DUTNAME).sym $(DUTNAME)_test.c > $@

$(TMPDIR)$(DUTNAME)_mocks.c: $(TMPDIR)$(DUTNAME).sym $(TMPDIR)$(DUTNAME)_data.h
	@$(SRCDIR)tmg $(TMPDIR)$(DUTNAME).sym $(TMPDIR)$(DUTNAME)_data.h > $@

$(TMPDIR)$(DUTNAME)_proxified.p: $(TMPDIR)$(DUTNAME).sym $(TMPDIR)$(DUTNAME).p
	@$(SRCDIR)tam $(TMPDIR)$(DUTNAME).sym $(TMPDIR)$(DUTNAME).p > $@

$(TMPDIR)$(DUTNAME)_runner.c: $(DUTNAME)_test.c $(TMPDIR)$(DUTNAME)_data.h
	@$(SRCDIR)ttg $(DUTNAME)_test.c $(TMPDIR)$(DUTNAME)_data.h > $@

$(TMPDIR)$(DUTNAME)_mocks.o: $(TMPDIR)$(DUTNAME)_mocks.c
	@$(CC) $(CFLAGS) $(TMPDIR)$(DUTNAME)_mocks.c

$(TMPDIR)$(DUTNAME)_proxified.o: $(TMPDIR)$(DUTNAME)_proxified.p
	@$(CC) $(CFLAGS) $(TMPDIR)$(DUTNAME)_proxified.p

$(DUTNAME)_test.o: $(DUTNAME)_test.c $(TMPDIR)$(DUTNAME)_data.h
	@$(CC) $(CFLAGS) $(DUTNAME)_test.c

$(TMPDIR)$(DUTNAME)_runner.o: $(TMPDIR)$(DUTNAME)_runner.c
	@$(CC) $(CFLAGS) $(TMPDIR)$(DUTNAME)_runner.c

$(DUTNAME)_test: $(DUTNAME)_test.o $(TMPDIR)$(DUTNAME)_proxified.o $(TMPDIR)$(DUTNAME)_runner.o $(TMPDIR)$(DUTNAME)_mocks.o
	@$(CC) $(CLFAGS) $(DUTNAME)_test.o $(TMPDIR)$(DUTNAME)_proxified.o $(TMPDIR)$(DUTNAME)_runner.o $(TMPDIR)$(DUTNAME)_mocks.o link to $(DUTNAME)_test

check_$(DUTNAME): $(DUTNAME)_test
        @$(DUTNAME)_test COMPACT

check: check_$(DUTNAME)

clean_$(DUTNAME):
        @delete QUIET $(TMPDIR)$(DUTNAME).p $(TMPDIR)$(DUTNAME).sym $(TMPDIR)$(DUTNAME)_data.h $(TMPDIR)$(DUTNAME)_mocks.c $(TMPDIR)$(DUTNAME)_proxified.p $(TMPDIR)$(DUTNAME)_runner.c $(TMPDIR)$(DUTNAME)_mocks.o $(TMPDIR)$(DUTNAME)_proxified.o $(TMPDIR)$(DUTNAME)_runner.o $(DUTNAME)_test.o $(DUTNAME)_test

clean: clean_$(DUTNAME)
