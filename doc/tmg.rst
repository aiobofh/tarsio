===
tmg
===

---------------------
Tarsio Mock Generator
---------------------

:Manual section: 3

SYNOPSIS
========

tmg [option] <symbol-cache.sym> <struct-header.h>

DESCRIPTION
===========

TMG stands for Tarsio Mock Generator.

tmg generate a source file to stdout. This source file contain mock-up
functions for all used functions in the design under test.

OPTIONS
=======

tmg only take two input file paths as arguments.

The input file should be generated with tcg(3).

--help, -h     Show help text
--version, -v  Print the version of tmg

EXAMPLES
========

If this would be the design under check::

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

The symbols and symbol usage detected by tcg(3) would enable tmg to generate
the something like following code::

  void  __tarsio_proxy_other_func(int a) {
    __tarsio_mock_data.other_func.args.arg0 = a;
    __tarsio_mock_data.other_func.call_count++;
    if (NULL != __tarsio_mock_data.other_func.func) {
      __tarsio_mock_data.other_func.func(a);
    }
  }

  void __tarsio_proxy_my_func(int a) {
    __tarsio_mock_data.my_func.args.arg0 = a;
    __tarsio_mock_data.my_func.call_count++;
    if (NULL != __tarsio_mock_data.my_func.func) {
      __tarsio_mock_data.my_func.func(__s);
    }
  }

These __tarsio_proxy* function will be called instead of the original ones
from check-cases. This enables the programmer to control the program flow
and sample arguments for asserts in checks.

SEE ALSO
========

tsg(3), tcg(3), tam(3), ttg(3), tarsio.mk(3)

BUGS
====

No known bugs so far. Please report them to the author.
