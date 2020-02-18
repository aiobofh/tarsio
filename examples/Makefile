export PATH:=../src:${PATH}

all: write_file_check
	./write_file_check

%.pp: %.c
	gcc -O0 -Wall -std=c11 -pedantic -g -I. -I../include -Dmain=__tarsio_replace_main -E -c $^ > $@

%.sym: %.pp
	tcg $^ $@

%_data.h: %.sym %_check.c
	tsg $^ > $@

%_mocks.c: %.sym %_data.h
	tmg $^ > $@

%_proxified.pp: %.sym %.pp
	tam $^ > $@

%_runner.c: %_check.c %_data.h
	ttg $^ > $@

%_proxified.o: %_proxified.pp
	gcc -Wall -std=c11 -pedantic -g -I. -I../include -x cpp-output -c -Wunused-function $< -o $@

%.o: %.c
	gcc -O0 -Wall -std=c11 -pedantic -g -I. -I../include -o $@ -c $<

%_check: %_proxified.o %_runner.o %_mocks.o %_check.o
	gcc -o $@ $^

clean:
	rm -f *.pp *.sym *_data.h *_mocks.c *_proxified.c *_runner.c *.o *_check *~
