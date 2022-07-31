/*
 * Tarsio Test-runner Generator
 *
 *              _______          _____ ___        ______
 *                 |      ||    |         |    | |      |
 *                 |      ||    |         |    | |      |
 *                 |   ___||___ |         |___ | |______|
 *
 *            Copyright (C) 2020-2022 AiO Secure Teletronics
 *
 * The Tarsio Test-runner Generator will output a C source file that can be
 * compiled into a runnable executable that will run all checks in a check-
 * suite in the declared order.
 *
 *  This file is part of Tarsio.
 *
 *  Tarsio is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Tarsio is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Tarsio.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "options.h"

#include "file.h"
#include "checkcase.h"

#include "version.h"

static char field[] = "$Id: ttg,v " VERSION " " __DATE__ " " __TIME__ " " AUTHOR " Exp $";
static char version[] = VERSION;
static char timestamp[] = __DATE__ " " __TIME__;

int __tarsio_debug_print = 0;

/****************************************************************************
 * Program usage
 */
static void usage(const char* program_name)
{
  printf("USAGE: %s [-n|--no-module] <check-file> <tarsio-pre-processed-source>\n", program_name);
}

/****************************************************************************
 * Program version
 */
static void ver(const char* program_name) {
  printf("%s v%s %s (%s)\n", program_name, version, timestamp, field);
}

/****************************************************************************
 * Option handling
 */
struct ttg_options_s {
  char* checkcases_filename;
  char* header_filename;
  int no_module;
};
typedef struct ttg_options_s ttg_options_t;

static int ttg_options_init(ttg_options_t* options, int argc, char* argv[])
{
  int rest;
  options_t ttg_options[4] = { {'v', "version", "VERSION", NULL, 0 },
                               {'h', "help", "?", NULL, 0 },
                               {'n', "no-module", "NOMODULE", NULL, 0},
                               {'d', "debug", "DEBUG", NULL, 0 } };

  if (0 > (rest = options_init(argc, argv, ttg_options, 4))) {
    usage(argv[0]);
    return -1;
  }

  if (ttg_options[0].enabled) {
    ver(argv[0]);
    return -1;
  }
  if (ttg_options[1].enabled) {
    usage(argv[0]);
    return -1;
  }
  if (ttg_options[2].enabled) {
    options->no_module = 1;
  }
  if (ttg_options[3].enabled) {
    __tarsio_debug_print = 1;
  }

  if ((argc - rest) != 3) {
    error2("ERROR: Illegal number (%d) of arguments (%d)", argc, (argc - rest));
    usage(argv[0]);
    return -1;
  }

  options->checkcases_filename = argv[rest + 1];
  options->header_filename = argv[rest + 2];

  return 0;
}

static int generate_check_runner(checkcase_list_t* list, const char* file, int no_module) {
  int retval = 0;
  checkcase_node_t* node;
  const char* data_file = file;
  char* ptr;

  puts("#include <stdio.h>\n"
       "#include <time.h>\n"
       "#include <stdlib.h>\n\n"
       "#include \"tarsio.h\"\n\n");
  while (NULL != (ptr = strstr(data_file, ":"))) {
    data_file = ptr + 1;
  }
  while (NULL != (ptr = strstr(data_file, "/"))) {
    data_file = ptr + 1;
  }
  while (NULL != (ptr = strstr(data_file, "\\"))) {
    data_file = ptr + 1;
  }

  printf("#include \"%s\"\n\n"
         "extern __tarsio_options_t __tarsio_options;\n\n", data_file);

  for (node = list->first; NULL != node; node = node->next) {
    printf("extern int __%s(void*, const char*);\n", node->name);
  }

  /*
   * Done with the silly stuff, on with the check-runner.
   */
  puts("\n"
       "__tarsio_data_t __tarsio_mock_data;");

  puts("\n"
       "void __tarsio_setup_mock_functions(void) {");
  if (0 == no_module) {
    puts("    TARSIO_DEFAULT_FUNCS");
  }
  puts("}");

  puts("\n"
       "int main(int argc, char* argv[]) {\n"
       "  int retval;\n"
       "\n"
       "  __tarsio_init();\n"
       "  __tarsio_handle_arguments(argc, argv);");
  for (node = list->first; NULL != node; node = node->next) {
    switch (node->type) {
    case CHECKCASE_IS_UNIT_CHECK:
      printf("  __tarsio_unit_check_execute(&__tarsio_mock_data, __%s, \"%s\", sizeof(__tarsio_mock_data));\n", node->name, node->name);
      break;
    case CHECKCASE_IS_MODULE_CHECK:
      printf("  __tarsio_module_check_execute(&__tarsio_mock_data, __%s, \"%s\", sizeof(__tarsio_mock_data));\n", node->name, node->name);
      break;
    case CHECKCASE_IS_UNKNOWN:
      error0("This should never happen :)");
      break;
    }
  }
  puts("  if (__tarsio_options.xml_output) {\n"
       "    retval = __tarsio_xml_output(argv[0], \"foobar\");\n"
       "  }\n"
       "  else {\n"
       "    retval = __tarsio_summary();\n"
       "  }\n"
       "  __tarsio_cleanup();\n"
       "  return retval;\n"
       "}\n");

  return retval;
}

int main(int argc, char* argv[]) {
  int retval = EXIT_SUCCESS;
  checkcase_list_t checkcase_list;
  checkcase_node_t* node;
  file_t check_file;
  ttg_options_t options;

  memset(&checkcase_list, 0, sizeof(checkcase_list));
  memset(&check_file, 0, sizeof(check_file));

  /*
   * Handle arguments passed to the program.
   */
  if (0 != ttg_options_init(&options, argc, argv)) {
    retval = EXIT_FAILURE;
    goto options_init_failed;
  }

  /*
   * Read the check-suite C-file.
   */
  if (0 != file_init(&check_file, options.checkcases_filename)) {
    retval = EXIT_FAILURE;
    goto read_check_file_failed;
  }

  /*
   * Extract check-cases
   */
  if (0 != checkcase_list_init(&checkcase_list, &check_file)) {
    retval = EXIT_FAILURE;
    goto checkcase_list_init_failed;
  }

  /*
   * Do not generate call to TARSIO_DEFAULT_FUNCS if no module checks are
   * declared. This reduce confusion in linking. Especially when there
   * are no object files for e.g. some weird embedded software when the
   * programmer only wants to use Tarsio for host-development. This is
   * achieved by simply setting the --no-module flag implicitly.
   */
  if (0 == options.no_module) {
    int found_module_checks = 0;
    for (node = checkcase_list.first; NULL != node; node = node->next) {
      if (CHECKCASE_IS_MODULE_CHECK == node->type) {
        found_module_checks = 1;
        break;
      }
    }
    if (0 == found_module_checks) {
      options.no_module = 1;
    }
  }

  /*
   * Generate check-runner
   */
  if (0 != generate_check_runner(&checkcase_list, options.header_filename, options.no_module)) {
    retval = EXIT_FAILURE;
    goto generate_check_runner_failed;
  }

 checkcase_list_init_failed:
  file_cleanup(&check_file);
 generate_check_runner_failed:
  checkcase_list_cleanup(&checkcase_list);
 read_check_file_failed:
 options_init_failed:
  return retval;
}
