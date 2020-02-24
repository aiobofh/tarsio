::

                 _______          _____ ___        ______
                    |      ||    |         |    | |      |
                    |      ||    |         |    | |      |
                    |   ___||___ |         |___ | |______|

                 Copyright (C) 2020 AiO Secure Teletronics


This is Tarsio
--------------

Tarsio is mainly developed to provide an auto-mocking, super-fast platform and
compiler agnostic unit checking framework for C-coders who find Check-Driven
Design a very useful tool.

Tarsio is released under the LGPLv3 license, see COPYING and COPYING.LESSER for
more information.

Tarsio is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Tarsio is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Tarsio.  If not, see <https://www.gnu.org/licenses/>.

Community
^^^^^^^^^

Tarsio source code is available on GitHub https://github.com/aiobofh/tarsio

IRC-channel for discussing Tarsio ircs://irc.aio.nu:6666/tarsio

Auto-mocking
^^^^^^^^^^^^

The Tarsio tool-chain provide means to automatically generate mock-up functions
for all functions/library calls you might use in your programs. It mimics the
function signature of every single function that is used and replace the calls
to the original functions and instead a kind-of controllable proxy function is
used instead. This is a mock-up. The programmer can then choose to just control
the return values (if applicable) or check that functions within your other
functions were called a specific number of times, and with what arguments. This
is done in a unit-check.


Super-fast
^^^^^^^^^^

Well... Maybe not ultra-fast, but super-fast. Tarsio rely on your compiler being
able to produce a source-file that is a pre-processed version of your design
under check. This means all pre-processor directives has been sorted out and only
pure C-code remain. Most compilers support this output format. Then in one pass
the Tarsio tool-chain is able to do all its magic by scanning source-files and
check cases to generate a check-runner that execute all your checks within a
suite and keep track of check verdicts.


Platform agnostic
^^^^^^^^^^^^^^^^^

The idea behind this is that most tools of this sort have quite a few
dependencies to other, 3rd party tools. These are not always available on the
platform you intend to work with, or are difficult to compile or port to that
specific system. Tarsio is written to compile with most C compilers, following
pretty old C standards and very few system calls to frameworks usually enjoyed
in the OS. This makes it highly portable.


Compiler agnostic
^^^^^^^^^^^^^^^^^

Since Tarsio operate on source-code level with figuring out what code is needed
to be generated to make life easier for you as a programmer. There are no binary
inspection, and no need for specific features in your favorite compiler, only
that is supports outputting a pre-processed version of your source code and that
the code is in a neat way. Too much poking around in the backwaters of your
compiler feature set will probably make things harder for Tarsio (and you) to
read your code.


Check-Driven Design
^^^^^^^^^^^^^^^^^^^

What is this wizardry!? One might ask. Or just wonder if I, the author, have a
spelling error on the word "development". Well. It is neither black magic, nor
a spelling error. There is such impact on how the code will look and be
organized using Check-Driven Development as a design tool. That is why the word
"Design" is chosen instead.


Components and artifacts
------------------------

This a simplified picture on what's going on and what artifacts and tools your
build system would need to take into account when compiling an executable check
runner from your C-source and Unit-checks::

 .-------------.   .------------.   .---------------.
 |  C-source   |   |   "Any"    |   | Pre-processed |
 |'Your design'|-->| C-compiler |-->|   C-source    |
 |   my_code.c |   |            |   |   my_code.pp  |
 '-------------'   '------------'   '---------------'
                                            |
                                            V
 .---------------.                  .----------------.
 |  Unit-checks  |                  | Tarsio command |
 |   C-source    |----------------->|   line tools   |
 |my_code_check.c|                  |   dark magic   |
 '---------------'                  '----------------'
   |                                    | |    | |
 .-'    .-------------------------------' |    | '-----------.
 |      |                .----------------' .--'             |
 |      |                |                  |                |
 |      V                V                  V                V
 | .---------------..--------------..----------------..--------------------.
 | | Mock-ups of   || Storestruct  ||   Check runner ||      Modified      |
 | | all function  || for every    ||      code      ||      C-source      |
 | | signatures    || mock-up      ||                ||                    |
 | |my_code_mocks.c||my_code_data.h||my_code_runner.c||my_code_proxified.pp|
 | '---------------''--------------''----------------''--------------------'
 |      |                |                 |                 |
 |      |                '-----. .---------'                 |
 |      '--------------------. | | .-------------------------'
 |                           | | | |
 |                           V V V V
 '---------------------->.--------------.
 .-----------------.     |    "Any"     |
 | tarsio run-time |---->|  C-compiler  |
 |     library     |     |  and linker  |
 |     tarsio.c    |     '--------------'
 '-----------------'            |
                                V
                         .--------------.      .------------.
                         |  Executable  |      | JUnit      |
                         | program with |----->| XML report |
                         |  check-suite |      |            |
                         '--------------'      '------------'

Tools
-----

The magic of Tarsio happens with help of the tool-set. The idea is to provide
small, fast and isolated tools that does what they're supposed to do, and do it
well.

TCG - Tarsio symbol Cache Generator
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This tool reads your source file (the design under test/check), and produce a
binary file with serialized data about all the function names and use of other
functions. For best result it should be given a pre-processed C source with all
the macros expanded and required files included. This is for example done with the
-E flag for GCC.

See the manual page for TCG for more information.

TSG - Tarsio Struct Generator
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To be able to store inspectable data for every mock-up function storage structs
can be generated using this tool. It takes your symbol file generated by TCG
of the source code you're writing, and the source code file containing your check
cases. It will output a header-file to stdout with all the data types required to
compile your check successfully.

See the manual page for TSG for more information.

TMG - Tarsio Mock Generator
^^^^^^^^^^^^^^^^^^^^^^^^^^^

The mock generator rely on the information generated by TCG (Tarsio symbol Cache
Generator) to automatically generate source code that emulates the API of all
functions used in your source code (the design under test/check). These mock-up
functions are controllable and can be considered as proxy-functions that can be
given various values for e.g. return value for a function that is declared to have
a return data type. The mocks can also be monitored for input arguments and also
call either the real function or hand-written subs or replacements for the
functions.

See the manual page for TMG for more information.

TAM - Tarsio Auto Mocker
^^^^^^^^^^^^^^^^^^^^^^^^

Auto-mocking is the concept of replacing the use of all functions in your source
code (design under test/check) to call the generated mock-up functions (proxy
functions) generated by the TMG (Tarsio Mock Generator) tool.

What this tool does is that it takes (a pre-processed) version of your source code
and textually replace every function call. So, for example if you have this code::

  #include <string.h>
  void my_func(const char* string) {
    printf("%s is %lu bytes long\n", string, strlen(string));
  }

Here there are a few function calls to printf() and strlen() both these calls will
be replaced with calls to the mock-up functions. Looking something like this::

  #include <string.h>
  void my_func(const char* string) {
    __tarsio_proxy_printf("%s is %lu bytes long\n", string,
                          __tarsio_proxy_strlen(string));
  }

But when running a debugger you will still see the original functions, as
expected. It's just that their functionality will be replaced with the generated
versions.

This procedure also requires some meta-information to be generated, for example:
Removal of static-declared functions, since they're gonna be called from your
check-cases, and extern declaration of function prototypes just around where they
are called.

See the manual page for TAM for more information.

TTG - Tarsio Test-runner Generator
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This tool takes the source code (design under test/check) generated by the TAM
(Tarsio Auto Mocker) tool, and your source-file containing your checks/tests as
input. It will output a new source file with a main() function and a run-schedule
for executing and evaluating all your check-cases in the order they are found in
the source file. So this is the source for compiling the actual executable runner
program. This is done to reduce boilerplate code needed to set-up the runner, and
clears states (that Tarsio know about) between every check-case so that they can
be run independently of each other.

Set the manual page for TTG for more information.

Building a suite using the tools
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

As mentioned earlier there are a lot of magic going on with help of the tools
described above. And all of them are needed to make a usable system to develop
your code check-driven. I (the author) usually prefer GNU Make to construct the
build-systems for my code so here is an annotated example on how to use Tarsio to
generate the required code and execute it for you.

Generate a serialized symbol cache
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This is an example on how to generate a symbol-file, it use a pre-processed
source file (`.pp`) that could be generated first (remember, this is a very
simplified Makefile-example)::

  my.pp: my.c
         gcc -E -o my.pp my.c

  my.sym: my.pp
         tcg my.pp my.sym

Now you will have the binary file that can be reused by some of the other tools.

Generate a check state data storage structure
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This file is to be included in your source code file containing the test/check
cases::

  my_data.h: my.sym my_check.c
         tsg my.sym my_check.c > my_data.h

This file should be included in your source code containing your check-cases.

Generate the mock-up functions for controlling your checks
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This step generate all the mock-up information in a file that is to be linked to
your check case runner::

  my_mocks.c: my.sym my_data.h
         tmg my.sym my_data.h > my_mocks.c

Generate a proxified version of your source code
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This step will rewrite your code to call only the mock-up functions generated in
the my_mocks.c file instead of the real functions::

  my_proxified.c: my.sym my.pp
         tam my.sym my.pp > my_proxified.c

Generate the check runner source code
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This step will generate a runable program (or rather the source code to a
runable program) that later can be compiled to an executable binary that executes
all the checks in the correct order::

  my_runner.c: my_check.c my_data.h
         ttg my_check.c my_data.h > my_runner.c

This was the last step to generate all the code needed to compile the runner.

NOTE: ttg can generate a runner without support for module-checks (checks that
      are not 100% stubbed, but rather call the original functions but still
      sample arguments and such things) by passing `-m` as first argument. This
      can make less "surprising" dependencies to real implementation object files.

Learn and become familiar with the tool-chain outputs
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

A good place to start to understand Tarsio is by playing around with the examples
folder where there are build-systems and sample code. When running `make` there
all artifacts are created and kept in the same folder for easy browsing.

A little bit more "real" example is to head over to the test-folder, where the
regression checkss for Tarsio itself live. However, the build system for Tarsio is
rather complex, to support various architectures, compilers and OS:es, so it can
be a bit daunting at first.

Writing a check
---------------

A unit-check in the Tarsio world is supposed to be a close to 100% isolated check
for the code that is the design under check. If Check-Driven Design is a new
concept - Please take a look in "The idea about Check-Driven Design".

In-between every check-case the storage data structure containing samples from
function call counters, argument monitors and function replacements (stubs) are
reset (set to zero). The thinking is that this enables every check to be
written as a state-less check. And there should not be a need to run checks in any
specific order. When the state is cleared it also means that every function call
you have in your code is replaced by a call to a mock-up function (mock).

The Tarsio tool chain supports yet another type of check - They are called
module-check. These are in essence exactly the same as a unit-check but with the
significant difference of clearing the sample storage state, but by default
setting all functions to be called as originally intended instead of just calling
the mock-up. To keep things on a basic level for now - Let's say if


Organizing the files
^^^^^^^^^^^^^^^^^^^^

The idea is to have a 1:1 mapping of check-suite (a file containing a bunch of
check-cases) and the file containing the functions to be checked. The build
system shipped with Tarsio also assumes that the files are named in a specific
way. This is however not a requirement for the Tarsio tool-chain it self. But
it might be a convenient way to keep track on what check-suite is checking
what code file.

Consider the case of having a source code file that is going to contain helper
functions for disk operations. To easily know what code is in the file by just
browsing the file tree, lets call the file ``disk_operations.c``. Then there
should be a matching check-suite called ``disk_operations_check.c`` containing
the check-cases. Again: This is given that the build systems shipped with
Tarsio are used.


Recommended directory layout
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Since the Tarsio tools and eventually code coverage files will be created it's
good to keep a separate ``test`` or ``check`` folder where the ``*_check.c``
suite files are kept. This could be inside a ``src`` folder or parallel with
the source-folder. It would also be a good idea to have a top-level build
system that enables the user of a project to easily build the system with all
regression checks run::

  Makefile
  src
    Makefile
    my_code.c
    helper.c
  check
    Makefile
    my_code_check.c
    helper_check.c

The focus for this text is do describe the ``check/Makefile`` structure. A good
way of making use of Tarsio would be something like this::

  #
  # My checks for my awesome project
  #

  #
  # Speedup GNU Make a bit
  #
  MAKEFLAGS += --no-builtin-rules
  .SUFFIXES:

  #
  # Default clean builds
  #
  CFLAGS=-Wall -pedantic -std=c99 -I.

  #
  # Configure Tarsio
  #
  TTMPROOT=/tmp/
  TSRCROOT=../src/

  #
  # Depend on the Tarsio make system
  #
  -include $(shell pkg-config --variable=includedir tarsio)/tarsio.mk

This set-up will find the sources in the ``src`` folder, and also store all the
various temporary/intermediate files that the Tarsio tools create in the
``/tmp`` folder. This is especially efficient if the ``/tmp`` folder is a RAM-
disk.

If your code ``my_code.c`` normally is linking to another object file when you
build the software in the usual way that object file also need to be linked to
the check-runner program. This is achieved by just extending the dependencies
to the make-target for the ``my_code_check`` target like so::

  #
  # Additinal linking information (for module-checks real code is linked)
  #
  my_code_check: ${TSRCROOT}helper.o

To run all the checks from inside the ``check`` folder just run::

  $ make

This will output a ``.`` for every check passed. ``F`` for every check failed
or if you used the ``skip()`` directive an ``S`` will be outputted. There are
other output modes available too. Read the ``tarsio.mk`` file for more info on
this.

Now to address top-level... If you want to make sure your product is always
running all checks before making a deliverable binary just chain a check target
as dependency to your normal ``all:`` target like so::

  .PHONY: all
  all: check
          ${MAKE} --no-print-directory -C src    # Build the src-folder

  .PHONY: check
  check:
          ${MAKE} --no-print-directory -C check  # Build and execute checks

... Or something similar.

Important includes
^^^^^^^^^^^^^^^^^^

When writing a new check-suite there are a few ``#include``-statements that are
mandatory. First of all you should ``#include`` the ``tarsio.h`` API, which
provide you with the functions and macros needed by the Tarsio tool chain.

Also... You probably want to include the ``disk_operations_data.h`` to get
access to all data types and function prototypes used in your design under check.
The ``disk_operations_data.h`` header file is generated by the ``tsg`` tool from
the Tarsio tool chain. See the manual for ``tsg`` for more information. This
filename can be whatever you wish, but in the case of the build-systems shipped
with Tarsio they named like this for convenience, and it's probably a good
practice to do so.

So you should probably have a file starting with the following lines::

  #include <tarsio.h>
  #include "disk_operations_data.h"

Some clarification is needed here. Your text-editor or Integrated Development
Environment may become a bit sad by this inclusion of the generated _data.h
file. Since it might not always exist... If your editor has some kind of simple
syntax validation some keywords used in your code might be marked as unavailable
or syntax errors. There are ways around this, for example if you generate the
file at some point (which hopefully will be every minute or so, when you get the
hang of Check-Driven Design). Then the text-editor should be a happy camper, most
of the time.


The simplest check
^^^^^^^^^^^^^^^^^^

A unit-check is defined by a macro that looks similar to a function prototype or
function header. The macro is called ``check()``, or in a module-check it is
called ``module_check()``, depending on which mocking behavior is desired::

 check(this_is_a_readable_check_name) {
   :
   My check code
   :
 }

A word of warning regarding check-names. Even though it is a very good idea to
name the checks to something valuable and understandable; some C compilers might
have constraints on the length of function names - Usually they are truncated in
this case, without warning, hence it's quite important to be aware of this.
Mainly since the usual pattern is to prefix the check-case name with the function
that is checked, and then some meaningful description of the check. Given this
knowledge, and if your functions under check them selves have meaningful long
names... All the checks might potentially end up being named the exact same thing
due to this truncation.

The number of lines of a unit-check often reflects on the complexity of the code
you are designing. It is a good thing to be aware of this basic rule of thumb:
If your check case is longer than 10-15 lines, your design under check should
probably be refactored, broken apart into smaller checkable items.

Again: See the full examples in "The idea about Check-Driven Design" on how to
incrementally build your application using the checks as your development
environment.


Compile'n'run
^^^^^^^^^^^^^

Once your check is ready to be compiled the first time. Just generate the needed
files and compile the check-runner. With the build system shipped with Tarsio
this is done by just building the ``check`` target. For example on a Linux
machine (and many others) in a console just type::

  $ make check

Or even::

  $ make

If you have applied the directory structure in such way that you have a special
folder for checks, that is separated from the code. You choose yourself in your
own build recipes.


The idea about Check-Driven Design
----------------------------------

If you have heard of check-driven development, and even better if you have
practiced it. You probably know what is intended with this semantic game of
changing development to design. If you are new to the concept. Please take your
time to think about what the underlying ideas of this programming paradigm are
trying to achieve.

Tarsio was created to make it as easy as possible to practice and hopefully
enjoy seeing your hack/application/game, or whatever you spend your time with
writing, evolve by writing your code check-driven.

Writing checks first::

         .---------------.                    .-------------------.
         |               V                    |                   V
   .------------.  .------------.     .----------------.  .----------------.
 .-| Write check|  | Write code |---->| Re-factor check|  | Re-factor code |-.
 | '------------'  '------------'     '----------------'  '----------------' |
 |       ^               |                    ^                   |          |
 |       '---------------'                    '-------------------'          |
 |                                                                           |
 |                           .-------------.                                 |
 '---------------------------| Delete check|<--------------------------------'
                             '-------------'

This might be a typical work-flow when always writing checks first, then
implement the code to make the check pass.

Tarsio is helping a programmer to design checks that are very small, and in that
way driving the design of the code.

Imagine an example where you are writing an application that is going to save
some kind of data to disk. This is a perfect example where removal of the
actual file access replacing it with mock-up functions or stubs is highly
recommended to both reduce check complexity, and run-time.

1. Write a check that makes sure that the function that is about to be
   implemented is opening the correct file for writing::

     check(shall_open_the_correct_file_for_writing) {
       write_file("some_file_path.dat");
       assert_eq(1, tarsio_mock.fopen.call_count);
       assert_eq(0, strcmp("some_file_path.dat", tarsio_mock.fopen.args.arg0);
       assert_eq(0, strcmp("w", tarsio_mock.fopen.args.arg1);
     }

2. Compile'n'run

   It will fail, since the write_file() function is not even implemented yet.

3. Implement the code

   A very naive implementation::

     void write_file(const char* filename) {
       (void)fopen(filename, "w");
     }

   The function call to fopen() will automatically be replaced by a call to a
   generated function by the Tarsio tools chain, hence we can measure how many
   calls we had to it, and what arguments were passed to it in the generated
   data-storage struct instance ``tarsio_data`` as the check case suggests.

   Something important to be aware of is that return values of function calls
   in a Tarsio processed file always return 0 - This makes it possible to write
   code and checks a bit cleverly, depending on which API is used in the code.
   Many C API's return 0 on success and a negative value on failure. Hence
   the program flow will ripple down through the code i many cases.

4. Compile'n'run

   The check shall now pass, even though this specific check might look very
   trivial and unnecessary it is an enabler for further design of the code.
   Especially making sure that other checks can be written with the knowledge
   that fopen is called correctly and can deal with various errors.

5. Write a check that makes it easy to know what is going wrong with the code
   if a file could not be opened for writing::

     check(shall_return_WRITE_FILE_FOPEN_FAILED_if_fopen_fails) {
       assert_eq(WRITE_FILE_FOPEN_FAILED, write_file("some_file_path.dat"));
     }

   Very compact, right... Since the check-suite runner that is generated by the
   Tarsio tool chain clears the data-storage struct ``tarsio_data`` in-between
   every check-case, it also clears the ``retval`` member. In this case NULL or
   0.

   While we're at it a check for the normal return value could also be useful
   to drive the check. Let's say that 0 is "everything is OK" return value.

   Now the manipulation of the ``tarsio_data`` storage struct is needed to
   make the call to ``fopen()`` return something known::

     check(shall_return_WRITE_FILE_EVERYTHING_IS_OK_if_everything_is_ok) {
       tarsio_mock.fopen.retval = (FILE*)0x1234;
       assert_eq(WRITE_FILE_EVERYTHING_IS_OK, write_file("some_file_path.dat"));
     }

6. Compile'n'run

   This will not even compile, since the original code implementation did not
   have a return-value, since it was a ``void`` function. So take this into
   consideration when implementing the code that will return ``-1`` if the
   call ``fopen()`` fails.

7. Implement the code

   One way of writing it::

     int write_file(const char* filename) {
       if (NULL == fopen(filename, "w")) {
         return WRITE_FILE_FOPEN_FAILED;
       }
       return WRITE_FILE_EVERYTHING_IS_OK;
     }

   or::

     int write_file(const char* filename) {
       int retval = WRITE_FILE_EVERYTHING_IS_OK;
       if (NULL == fopen(filename, "w")) {
         retval = WRITE_FILE_FOPEN_FAILED;
       }
       return retval;
     }

   or even::

     int write_file(const char* filename) {
       int retval = WRITE_FILE_EVERYTHING_IS_OK;
       if (NULL == fopen(filename, "w")) {
         retval = WRITE_FILE_FOPEN_FAILED;
         goto fopen_failed;
       }
       goto everything_is_ok;

      fopen_failed:
      everything_is_ok:
       return retval;
     }

   The code is still valid and as a programmer you are free to use any style
   you can come up with. The different styles have different charms and
   underlying religious. Tarsio does not care - Just make it readable and
   easy to refactor, self-documenting or whatever you feel like.

8. Compile'n'run

   The checks shall now pass. As you can see, they execute extremely fast,
   since the actual code writing the file to disk is not even called and the
   program flow can now be controlled from the check-cases with very few lines
   of code.

9. Write a few checks to make sure that the *correct* file handle is closed

   ... And only if it actually was opened::

     check(shall_close_the_correct_file_if_opened) {
       tarsio_mock.fopen.retval = (FILE*)0x1234;
       write_file("some_file_path.dat");
       assert_eq(1, tarsio_mock.fclose.call_count);
       assert_eq((FILE*)0x1234, tarsio_mock.fclose.args.arg0);
     }

     check(shall_not_close_a_file_by_accided_if_file_was_not_opened) {
       write_file("some_file_path.dat");
       assert_eq(0, tarsio_mock.fclose.call_count);
     }

10. Compile'n'run

    The this check will not even compile. Since the Tarsio tool-chain did not even
    find any calls to ``fclose()``. Hence the ``tarsio_data`` struct will not
    even contain the member ``fclose`` as sample data for the asserts in the
    check.

11. Implement the code

    The early exit code style::

     int write_file(const char* filename) {
       FILE* fd;
       if (NULL == (fd = fopen(filename, "w"))) {
         return WRITE_FILE_FOPEN_FAILED;
       }
       fclose(fd);
       return WRITE_FILE_EVERYTHING_IS_OK;
     }

    or the if/else style::

     int write_file(const char* filename) {
       int retval = WRITE_FILE_EVERYTHING_IS_OK;
       FILE* fd = NULL;
       fd = fopen(filename, "w")
       if (NULL != fd) {
         fclose(fd);
       }
       else {
         retval = WRITE_FILE_FOPEN_FAILED;
       }
       return retval;
     }

    or even self-documenting goto style::

     int write_file(const char* filename) {
       int retval = WRITE_FILE_EVERYTHING_IS_OK;
       FILE* fd;
       if (NULL == (fd = fopen(filename, "w"))) {
         retval = WRITE_FILE_FOPEN_FAILED;
         goto fopen_failed;
       }
       fclose(fd);
       goto everything_is_ok;
      fopen_failed:
      everything_is_ok:
       return retval;
     }

12. Compile'n'run

    Now there is some error-recovery in place, and also good and understandable
    return values. All checks should still pass.

    A small tip - For free, to have a good self-documenting code style
    regardless of your preferred code aesthetics is to actually name the return
    values to something meaningful, which might be important in the non-goto
    style versions::

     typedef enum {
       WRITE_FILE_EVERYTHING_IS_OK = 0,
       WRITE_FILE_FOPEN_FAILED = -1
     } write_file_rt;

    ... and change the return value type from ``int`` to write_file_rt. And if
    you are clever this can also be used in the check-cases to give them even
    more self-documenting features. It's up to you the coding master.

    In this case the check-cases are refactored first, and then the code, with
    the exact same mind-set as the initial implementation.

13. Write a check that makes sure all data is written do disk.

    Here comes a bit trickier refactoring, along with new implementation. Since
    there is no data passed to the function yet. More arguments have to be
    added, and all existing checks need to be refactored to take these in to
    account. But if things are designed in a good way this should be quite easy
    and in most cases the new arguments can be disregarded completely, since we
    are doing white-box checking and know the program flow (and have it verified
    by the checks written)::

     check(shall_open_the_correct_file_for_writing) {
       write_file("some_file_path.dat", NULL, 0);
       :
       Same as before
       :
     }

     check(return_WRITE_FILE_FOPEN_FAILED_if_fopen_fails) {
       assert_eq(WRITE_FILE_FOPEN_FAILED, write_file("some_file_path.dat", NULL, 0));
     }

    This one need to be given some extra thought, since something is probably
    going to be written, if everything is OK. Let's just pass some bogus data
    to the function::

     check(shall_return_WRITE_FILE_EVERYTHING_IS_OK_if_everything_is_ok) {
       tarsio_mock.fopen.retval = (FILE*)0x1234;
       tarsio_mock.fwrite.retval = 10;
       assert_eq(WRITE_FILE_EVERYTHING_IS_OK, write_file("some_file_path.dat", (void*)0x5678, 10));
     }

     check(write_file_shall_close_the_correct_file_if_opened) {
       tarsio_mock.fopen.retval = (FILE*)0x1234;
       write_file("some_file_path.dat", (void*)0x5678, 10);
       :
       Same as before
       :
     }

     check(shall_not_close_a_file_by_accident_if_file_was_not_opened) {
       write_file("some_file_path.dat", NULL, 0);
       assert_eq(0, tarsio_mock.fclose.call_count);
     }

    Also - The new checks for actually writing the data passed to ``write_file()``
    can be written.

    First a small check, to make sure that ``fwrite`` is writing the correct data
    to the correct file, by manipulating the retval of ``fopen()`` as before, to
    get a known value that should be passed to ``fwrite``::

     check(write_the_data_to_the_correct_file) {
       tarsio_mock.fopen.retval = (FILE*)0x1234;
       write_file("some_file_path.dat", (void*)0x5678, 10);
       assert_eq(1, tarsio_mock.fwrite.call_count);
       assert_eq((void*)0x5678, tarsio_mock.fwrite.args.arg0);
       assert_eq(1, tarsio_mock.fwrite.args.arg1);
       assert_eq(10, tarsio_mock.fwrite.args.arg2);
       assert_eq((FILE*)0x1234, tarsio_mock.fwrite.args.arg3);
     }

    And obviously it can be good to have a check that makes sure that the code
    will not write anything to somewhere that was never opened::

     check(write_file_should_not_write_data_if_fopen_failed) {
       write_file("some_file_path.dat", NULL, 0);
       assert_eq(0, tarsio_mock.fwrite.call_count);
     }

    It is always a good idea also to take error handling into account for new
    code added... So let's also write a few check that makes sure that the
    function return something meaningful if ``fwrite`` should fail - For example
    if a disk breaks during the write or a network file-system is suddenly
    unavailable during the write. This would make the code a bit more robust::

     check(return_WRITE_FILE_FWRITE_FAILED_if_file_write_fails) {
       tarsio_mock.fopen.retval = (FILE*)0x1234;
       assert_eq(WRITE_FILE_FWRITE_FAILED, write_file("some_file_path.dat", (void*)0x5678, 10));
     }

    Again - The Tarsio framework has NULL:ed the retval of fwrite automatically
    so it will return 0 bytes written.

    It is also good to know that the file is closed even if fwrite failed, but
    this is actually already covered in the generic assumption that if a fopen
    is successful, the file should also be closed, always.

14. Compile'n'run

    As you might have figured out. This won't compile, since the function has
    not been refactored to take three arguments yet, nor does it call ``fwrite``.

15. Implement the code

    By adding the new arguments to the data and its size, along with writing the
    data to file in the correct place in your code it would probably look
    something like this:

    The early exit code style::

     int write_file(const char* filename, void* data, size_t size) {
       FILE* fd;
       if (NULL == (fd = fopen(filename, "w"))) {
         return WRITE_FILE_FOPEN_FAILED;
       }
       if (size != fwrite(data, 1, size, fd)) {
         fclose(fd);
         return WRITE_FILE_FWRITE_FAILED;
       }
       fclose(fd);
       return WRITE_FILE_EVERYTHING_IS_OK;
     }

    or the if/else style::

     int write_file(const char* filename, void* data, size_t size) {
       int retval = WRITE_FILE_EVERYTHING_IS_OK;
       FILE* fd = NULL;
       fd = fopen(filename, "w")
       if (NULL != fd) {
         if (size != fwrite(data, 1, size, fd)) {
           retval = WRITE_FILE_FWRITE_FAILED;
         }
         fclose(fd);
       }
       else {
         retval = WRITE_FILE_FOPEN_FAILED;
       }
       return retval;
     }

    or even goto style::

     int write_file(const char* filename, void* data, size_t size) {
       int retval = WRITE_FILE_EVERYTHING_IS_OK;
       FILE* fd;
       if (NULL == (fd = fopen(filename, "w"))) {
         retval = WRITE_FILE_FOPEN_FAILED;
         goto fopen_failed;
       }
       if (size != fwrite(data, 1, size, fd)) {
         retval = WRITE_FILE_FWRITE_FAILED;
         goto fwrite_failed;
       }
      fwrite_failed:
       fclose(fd);
       goto everything_is_ok;
      fopen_failed:
      everything_is_ok:
       return retval;
     }

16. Compile'n'run

    Now the ``write_file`` function is fairly well unit-checked, and the design
    of the code was fully driven by the checks that was written before the code.

17. Given the fact that something *could* fail during write, might also
    indicate that even the ``fclose()`` could fail, let's check this too...

    First off, a meaningful return code to distinguish a ``fclose()`` failure
    from other failures would probably be nice::

      check(return_WRITE_FILE_FCLOSE_FAILED_if_file_could_not_be_closed) {
        tarsio_mock.fopen.retval = (FILE*)0x1234;
        tarsio_mock.fclose.retval = EOF;
        assert_eq(WRITE_FILE_FCLOSE_FAILED, write_file("some_file_path.dat", (void*)0x5678, 10));
      }

    Ok... Now we enter an interesting problem. Some coding styles have multiple
    calls to the fclose. Depending on branch state and such things. In essence,
    we can write a check that makes sure that the code can return -3 regardless
    of which branch we enter, or we just pick a coding style that is most
    generic and the easiest to check.

    It's up to you... The check where fclose is called if fopen was successful
    might not suffice anymore. But if you're clever by refactoring the code you
    may not have to write this check.

18. Compile'n'run

    The check should fail. Since the code has not been implemented yet.

19. Implement the code

    The early exit code style::

     int write_file(const char* filename, void* data, size_t size) {
       FILE* fd;
       if (NULL == (fd = fopen(filename, "w"))) {
         return WRITE_FILE_FOPEN_FAILED;
       }
       if (size != fwrite(data, 1, size, fd)) {
         if (0 != fclose(fd)) {
           return -3;
         }
         return WRITE_FILE_FWRITE_FAILED;
       }
       if (0 != fclose(fd)) {
         return WRITE_FILE_FCLOSE_FAILED;
       }
       return 0;
     }

    or the if/else style::

     int write_file(const char* filename, void* data, size_t size) {
       int retval = 0;
       FILE* fd = NULL;
       fd = fopen(filename, "w")
       if (NULL != fd) {
         if (size != fwrite(data, 1, size, fd)) {
           retval = WRITE_FILE_FWRITE_FAILED;
         }
         if (0 != fclose(fd)) {
           retval = WRITE_FILE_FCLOSE_FAILED;
         }
       }
       else {
         retval = WRITE_FILE_FOPEN_FAILED;
       }
       return retval;
     }

    or even goto style::

     int write_file(const char* filename, void* data, size_t size) {
       int retval = 0;
       FILE* fd;
       if (NULL == (fd = fopen(filename, "w"))) {
         retval = WRITE_FILE_FOPEN_FAILED;
         goto fopen_failed;
       }
       if (size != fwrite(data, 1, size, fd)) {
         retval = WRITE_FILE_FWRITE_FAILED;
         goto fwrite_failed;
       }
      fwrite_failed:
       if (0 != fclose(fd)) {
         retval = WRITE_FILE_FCLOSE_FAILED;
       }
       goto everything_is_ok;
      fopen_failed:
      everything_is_ok:
       return retval;
     }

20. Compile'n'run

    Now we have a ready function that fulfills the check-cases and everything
    seems OK.

21. Refactor the code to be more effective than religiously efficient

    In this case, just to reduce duplicated code the remaining design is the
    goto version, not that it is better or worse than anything else. It just
    suited the needs of the check-case this time. Also, there was no real need
    to do a goto for the fopen failure, hence early exit is used there. So
    in order to just fulfill the checks the code can look very different. And
    also thinking about what the code should do in different states makes it
    easier to refactor and restructure it to be readable or efficient,
    depending on the current needs.

    Just sit back, and fiddle with the code - All the checks needed are already
    in place, and you should be able to be creative to maintain check constraints
    while making the code extremely easy to understand::

      int write_file(const char* output_filename, void* data, size_t size) {
        const FILE* fd = fopen(output_filename, "w");

        if (NULL == fd) {
          return WRITE_FILE_FOPEN_FAILED;
        }

        const size_t result = fwrite(data, 1, size, fd);
        const int fwrite_ok = (size == result);

        if (0 != fclose(fd)) {
          return WRITE_FILE_FCLOSE_FAILED;
        }

        if (!fwrite_ok) {
          return WRITE_FILE_FWRITE_FAILED;
        }

        return RETURN_EVERYTHING_IS_OK;
      }

    Even though the code above is not compatible with older compilers, nor is
    it consistent in code style. But it shows ONE end-result that is quite
    compact, while still being easy to read. Also it does comply to all the
    checks we've written.

22. In the best of worlds it should be possible to write some kind of
    acceptance check already from the start. But it will probably not drive the
    design of the code so much... But rather set the functional constraints of
    the code that is to be implemented. In this case it can also help to design
    the API early, to reduce the need to refactor check cases, to that regard.

    But it can also inhibit the design flow a bit. Anyhow, here is an example
    of what that might look like. They might add more value as regression checks
    and or integration checks for different OS:es and platforms, hence they
    could also be written afterwords - depending on your preferences::

      module_check(write_file_should_successfully_write_data_to_disk) {
        char* data = "0123456789";
        assert_eq(0, write_file("/tmp/foo.dat", data, strlen(data)));
        assert_eq(0, strcmp(tarsio_mock.fopen.args.arg0, "/tmp/foo.dat"));
        char* result = read_file("/tmp/foo.dat");
        assert_eq(0, strcmp(data, result));
        free(result);
        unlink("/tmp/foo.dat");
      }

      module_check(write_file_should_fail_if_file_can_not_be_opened) {
        char* data = "0123456789";
        tarsio_mock.fopen.func = NULL;
        tarsio_mock.fopen.retval = NULL;
        assert_eq(WRITE_FILE_FOPEN_FAILED, write_file("/tmp/foo.dat", data, 10));
      }

      module_check(write_file_should_fail_if_file_can_not_be_written) {
        char* data = "0123456789";
        tarsio_mock.fwrite.func = NULL;
        tarsio_mock.fwrite.retval = 0;
        assert_eq(WRITE_FILE_FWRITE_FAILED, write_file("/tmp/foo.dat", data, 10));
        unlink("/tmp/foo.dat");
      }

      module_check(write_file_should_fail_if_file_can_not_be_closed) {
        char* data = "0123456789";
        tarsio_mock.fclose.func = NULL;
        tarsio_mock.fclose.retval = EOF;
        assert_eq(WRITE_FILE_FCLOSE_FAILED, write_file("/tmp/foo.dat", data, 10));
        unlink("/tmp/foo.dat");
      }

    Once these are written you basically have everything needed to do
    the code, that's why it might be a good idea to wait (for THIS example).
    For a couple for reasons. These checks promote an up-front planning of the
    code design - which is not an agile mind-set. Also... They will probably
    let a few design-pit-falls slip by... And you will probably end-up with
    code that is slightly different. It may not be bad, nor good. Just
    different.
