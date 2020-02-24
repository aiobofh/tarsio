===
ttg
===

----------------------------
Tarsio Test-runner Generator
----------------------------

:Manual section: 3

SYNOPSIS
========

ttg [option] <check-suite.c> <pre-processed-source-code.pp>

DESCRIPTION
===========

TTG stands for Tarsio Test-runner Generator

ttg generate a source file to stdout, containing a main() program that
executes each check in the order that they are defined in the check-suite.

OPTIONS
=======

ttg only take two input files paths as arguments.

--help, -h       Show help text
--version, -v    Print the version of ttg
--no-module, -n  Do not set function pointers to real code in module_checks
                 when the binaries are not available on the check-host. This
                 will make it possible to check things, but replace target
                 specific code with stubs in module checks.

EXAMPLES
========

If this would be the check-suitek::

  chech(some_check) {
    :
    Code
    :
  }

  check(another_check) {
    :
    Code
    :
  }

The output file will generate a main() function looking somewhat like this::

  int main(int argc, char* argv[]) {
    __tarsio_init();
    __tarsio_handle_arguments(argc, argv);
    __tarsio_unit_check_execute(__some_check, "some_check");
    __tarsio_unit_check_execute(__another_check, "another_check");
    __tarsio_xml_output(argv[0]);
    return __tarsio_summary();
  }

This is however quite hidden in the build chains. All the actual
implementation used here is hidden in the tarsio.c file.

SEE ALSO
========

tsg(3), tcg(3), tmg(3), tam(3), tarsio.mk(3)

BUGS
====

No known bugs so far. Please report them to the author.
