===
tsg
===

-----------------------
Tarsio Struct Generator
-----------------------

:Manual section: 3

SYNOPSIS
========

tsg [option] <symbol-cachee.sym> <check_suite.c>

DESCRIPTION
===========

TSG stands for Tarsio Struct Generator.

tsg generates a header-file to stdout, to be included in a check-suite,
which holds a declaration of the mock-up function storage struct. This is
required to get hold of the ``tarsio_mock`` variable for controlling or
evaluating calls to mock-up functions in a check.

This tool is usually invoked from a build-system as an early step to enable
the other "Tarsio"-tools to be able to generate various C source code files
and header files.

OPTIONS
=======

tsc only take two input files and the output is sent to stdout.

--help, -h     Show help text
--version, -v  Print the version of tsg

EXAMPLES
========

Here is an example of what to expect:

The tsg tool will generate the tarsio_mock variable to be an instance of a
struct somewhat like this::

  struct {
    struct {
      int call_count;
      void (*func)(int /* a */);
      struct {
        int arg0; /* a */
      } args;
    } other_func;
    struct {
      int call_count;
      void (*func)(int /* a */);
      struct {
        int arg0; /* a */
      } args;
    } my_func;
  } tarsio_mock;

Imagine it's now possible to have this check::

  check(my_func_should_pass_argument_plus_one_to_other_func) {
    my_func(5);
    assert_eq(6, tarsio_mock.other_func.args.arg0);
  }

And the design under check looks like this::

  static void other_func(int a) {
    :
    Do something
    :
  }

  static void my_func(int a) {
    :
    Do something
    :
    other_func(a + 1);
  }

With the help of other generated code the tarsio_mock variable will be set
to values, as shown in the check()-example above. The ``func`` member is a
function pointer to the equivalence of any used function, and can be set to
point to a stub or emulator for that functionality.

SEE ALSO
========

tcg(3), tmg(3), tam(3), ttg(3), tarsio.mk(3)

BUGS
====

No known bugs so far. Please report them to the author.
