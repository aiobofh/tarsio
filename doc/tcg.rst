tcg
===

Tarsio symbol-Cache Generator

SYNOPSIS
--------

tcg inputfile.pp outputfile.sym

DESCRIPTION
-----------

TCG stands for Tarsio symbol-Cache Generator.

tgc generate a binary file with all the symbols that can be parsed from a
pre-processed C-source file.

Only information relevant for generating other files with the automated
testing framework "Tarsio" are stored in this file and the file-format may
change between versions of the "Tarsio" unit-testing frame-work.

This tool is usually invoked from a build-system as an early step to enable
the other "Tarsio"-tools to be able to generate various C source code files
and header files.

The output file contain a great deal more than just the symbols - It will
also hold complete structures for function prototypes and the data types
needed by the other tools.

OPTIONS
-------

tgc only take input/output file paths as aguments.

SEE ALSO
--------

tsg(1), tmg(1), tam(1), ttg(1), tarsio.mk

BUGS
----

No known bugs so far. Please report them to the author.