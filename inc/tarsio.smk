#
# SMakefile to for building a complete check-suite using the Tarsio tool-chain
#
#               _______          _____ ___        ______
#                  |      ||    |         |    | |      |
#                  |      ||    |         |    | |      |
#                  |   ___||___ |         |___ | |______|
#
#             Copyright (C) 2020-2022 AiO Secure Teletronics
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
# Since smake is a bit different than modern GNU Make a few more manual
# targets need to be defined and called upon explicitly:
#
# Example smakefile:
#
# INCDIR=INCLUDE:tarsio/tarsio.smk
# all:
#   @smake -s -f $(INCDIR)tarsio.smk my_code_test DUTNAME=my_code CPU=$(CPU)
#   @smake -s -f $(INCDIR)tarsio.smk check DUTNAME=my_code CPU=$(CPU)
#   @echo ""
#
# This file requires the CPU variable to be set. It should be passed to here
# from your check-folder's smakefile.

SRCDIR=../src/
INCDIR=../inc/
TSTDIR=../test/
COVDIR=T:
TMPDIR=T:

CFLAGS=CPU $(CPU) define NODEBUG define SASC optimize noicons noversion includedirectory $(TMPDIR) includedirectory $(INCDIR) includedirectory $(SRCDIR)

$(TMPDIR)$(DUTNAME).p: $(SRCDIR)$(DUTNAME).c
	@$(CC) $(CFLASG) $(SRCDIR)$(DUTNAME).c preprocessoronly
	@copy $(SRCDIR)$(DUTNAME).p $(TMPDIR)$(DUTNAME).p
	@delete QUIET $(SRCDIR)$(DUTNAME).p $(SRCDIR)$(DUTNAME).p.info

$(TMPDIR)$(DUTNAME).sym: $(TMPDIR)$(DUTNAME).p
	@$(SRCDIR)tcg $(TMPDIR)$(DUTNAME).p $(TMPDIR)$(DUTNAME).sym

$(TMPDIR)$(DUTNAME)_data.h: $(TMPDIR)$(DUTNAME).sym $(DUTNAME)_test.c
	@$(SRCDIR)tsg $(TMPDIR)$(DUTNAME).sym $(DUTNAME)_test.c > $@

$(TMPDIR)$(DUTNAME)_mocks.c: $(TMPDIR)$(DUTNAME).sym $(TMPDIR)$(DUTNAME)_data.h
	@$(SRCDIR)tmg $(TMPDIR)$(DUTNAME).sym $(TMPDIR)$(DUTNAME)_data.h > $@

$(TMPDIR)$(DUTNAME)_proxified.p: $(TMPDIR)$(DUTNAME).sym $(TMPDIR)$(DUTNAME).p $(TMPDIR)$(DUTNAME)_data.h
	@$(SRCDIR)tam $(TMPDIR)$(DUTNAME).sym $(TMPDIR)$(DUTNAME).p $(TMPDIR)$(DUTNAME)_data.h > $@

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

$(TMPDIR)tarsio.o: $(SRCDIR)tarsio.c
        @$(CC) $(CFLAGS) $(SRCDIR)tarsio.c
	@copy $(SRCDIR)tarsio.o $(TMPDIR)tarsio.o
	@delete QUIET $(SRCDIR)tarsio.o

$(DUTNAME)_test: $(DUTNAME)_test.o $(TMPDIR)$(DUTNAME)_proxified.o $(TMPDIR)$(DUTNAME)_runner.o $(TMPDIR)$(DUTNAME)_mocks.o $(TMPDIR)tarsio.o
        @$(CC) $(CLFAGS) $(DUTNAME)_test.o $(TMPDIR)$(DUTNAME)_proxified.o $(TMPDIR)$(DUTNAME)_runner.o $(TMPDIR)$(DUTNAME)_mocks.o $(TMPDIR)tarsio.o link to $(DUTNAME)_test

check_$(DUTNAME): $(DUTNAME)_test
        @$(DUTNAME)_test COMPACT

check: check_$(DUTNAME)

clean_$(DUTNAME):
        @delete QUIET $(TMPDIR)$(DUTNAME).p $(TMPDIR)$(DUTNAME).sym $(TMPDIR)$(DUTNAME)_data.h $(TMPDIR)$(DUTNAME)_mocks.c $(TMPDIR)$(DUTNAME)_proxified.p $(TMPDIR)$(DUTNAME)_runner.c $(TMPDIR)$(DUTNAME)_mocks.o $(TMPDIR)$(DUTNAME)_proxified.o $(TMPDIR)$(DUTNAME)_runner.o $(DUTNAME)_test.o $(TMPDIR)tarsio.o $(DUTNAME)_test $(DUTNAME)_test.info $(DUTNAME)_test.lnk

clean: clean_$(DUTNAME)
