===
tcg
===

-----------------------------
Tarsio symbol-Cache Generator
-----------------------------

:Manual section: 3

SYNOPSIS
========

tcg [option] <pre-processed-source.pp> <symbol-cache.sym>

DESCRIPTION
===========

TCG stands for Tarsio symbol-Cache Generator.

tcg generate a binary file with all the symbols that can be parsed from a
pre-processed C-source file.

Only information relevant for generating other files with the automated
checking framework "Tarsio" are stored in this file and the file-format may
change between versions of the "Tarsio" unit-checking framework.

This tool is usually invoked from a build-system as an early step to enable
the other "Tarsio"-tools to be able to generate various C source code files
and header files.

The output file contain a great deal more than just the symbols - It will
also hold complete structures for function prototypes and the data types
needed by the other tools.

OPTIONS
=======

tgc only take input/output file paths as arguments.

The input file should be a C-pre-processed C source file (produced with the
-E flag with GCC for example).

The output file is a file containing all the symbols and usage of them. The
file format is Tarsio internal, and this file enables speedups and state
storage between different Tarsio tools.

--help, -h     Show help text
--version, -v  Print the version of tcg

EXAMPLES
========

This is how to provide the correct input for the tcg tool::

  gcc -O0 --no-inline -Dmain=__tarsio_replace_main -E -c my.c > my.i
  tcg my.i my.sym

The my.sym is the result file, used for most other Tarsio tools.

SEE ALSO
========

tsg(3), tmg(3), tam(3), ttg(3), tarsio.mk(3)

BUGS
====

No known bugs so far. Please report them to the author.
