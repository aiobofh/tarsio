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
        @$(DUTNAME)_test

check: check_$(DUTNAME)

clean_$(DUTNAME):
        @delete QUIET $(TMPDIR)$(DUTNAME).p $(TMPDIR)$(DUTNAME).sym $(TMPDIR)$(DUTNAME)_data.h $(TMPDIR)$(DUTNAME)_mocks.c $(TMPDIR)$(DUTNAME)_proxified.p $(TMPDIR)$(DUTNAME)_runner.c $(TMPDIR)$(DUTNAME)_mocks.o $(TMPDIR)$(DUTNAME)_proxified.o $(TMPDIR)$(DUTNAME)_runner.o $(DUTNAME)_test.o $(DUTNAME)_test

clean: clean_$(DUTNAME)
