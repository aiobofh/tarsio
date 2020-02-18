/*
 * Tarsio Struct Generator
 *
 *              _______          _____ ___        ______
 *                 |      ||    |         |    | |      |
 *                 |      ||    |         |    | |      |
 *                 |   ___||___ |         |___ | |______|
 *
 *              Copyright (C) 2020 AiO Secure Teletronics
 *
 * This program generates storage structures for check-suites to maniupulate
 * and sample mock-up function statistics.
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

#include "file.h"
#include "prototype.h"
#include "cpp.h"
#include "symbol_cache.h"

#include "version.h"

static char field[] = "$Id: tsg,v " VERSION " " __DATE__ " " __TIME__ " " AUTHOR " Exp $";
static char version[] = VERSION;
static char timestamp[] = __DATE__ " " __TIME__;

/****************************************************************************
 * Program usage
 */
static void usage(const char* program_name)
{
  printf("USAGE: %s <pre-processed-source> <test-suite>\n", program_name);
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
struct tsg_options_s {
  char* cache_filename;
  char* test_filename;
};
typedef struct tsg_options_s tsg_options_t;

static int tsg_options_init(tsg_options_t* options, int argc, char* argv[])
{
  if (argc == 2) {
    if ((0 == strcmp("-v", argv[1])) || (0 == strcmp("--version", argv[1])) || (0 == strcmp("VERSION", argv[1]))) {
      ver(argv[0]);
      return -1;
    }
    if ((0 == strcmp("-h", argv[1])) || (0 == strcmp("--help", argv[1])) || (0 == strcmp("?", argv[1]))) {
      usage(argv[0]);
      return -1;
    }
  }

  if (argc != 3) {
    error1("ERROR: Illegal number (%d) of arguments", argc);
    usage(argv[0]);
    return -1;
  }

  options->cache_filename = argv[1];
  options->test_filename = argv[2];

  return 0;
}

/****************************************************************************
 * Mock structure output
 *
 * TODO: Clean this shit up a bit
 */
static void generate_struct(prototype_list_t* list, cpp_list_t* cpp_list) {
  prototype_node_t* node;
  cpp_node_t* cpp_node;
  printf("/*\n"
         " * Tarsio mock data storage struct - This file is generated by tsg\n"
         " *\n"
         " *              _______          _____ ___        ______\n"
         " *                 |      ||    |         |    | |      |\n"
         " *                 |      ||    |         |    | |      |\n"
         " *                 |   ___||___ |         |___ | |______|\n"
         " *\n"
         " *              Copyright (C) 2020 AiO Secure Teletronics\n"
         " *\n");
  printf(" * Header file for use as inclusion describing all the mock control\n"
         " * structures and data storeage for mocked version of all used functions\n"
         " * in the design under test.\n"
         " *\n"
         " *  This file is part of Tarsio.\n"
         " *\n"
         " *  Tarsio is free software: you can redistribute it and/or modify\n"
         " *  it under the terms of the GNU General Public License as published by\n"
         " *  the Free Software Foundation, either version 3 of the License, or\n"
         " *  (at your option) any later version.\n"
         " *\n"
         " *  Tarsio is distributed in the hope that it will be useful,\n"
         " *  but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
         " *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
         " *  GNU General Public License for more details.\n"
         " *\n"
         " *  You should have received a copy of the GNU General Public License\n"
         " *  along with Tarsio.  If not, see <https://www.gnu.org/licenses/>.\n"
         " *\n"
         " */\n"
         "\n");
  printf("#ifndef _TARSIO_DATA_\n"
         "#define _TARSIO_DATA_\n"
         "\n");
  for (cpp_node = cpp_list->first; NULL != cpp_node; cpp_node = cpp_node->next) {
    printf("%s\n", cpp_node->info.directive);
  }
  printf("\n"
         "#define TARSIO_DEFAULT_FUNCS");
  for (node = list->first; NULL != node; node = node->next) {
    printf(" \\\n  __tarsio_mock_data->%s.func = %s;", node->info.symbol, node->info.symbol);
  }
  printf("\n");
  printf("\n"
         "struct __tarsio_data_s {\n"
         "\n");
  if (NULL == list->first) {
    printf("  int placeholder;\n");
  }
  for (node = list->first; NULL != node; node = node->next) {
    argument_node_t* anode;
    int cnt = 0;
    int i;
    printf("  struct {\n"
           "    int call_count;\n");

    /*
     * retval
     */
    if ((0 != node->info.datatype.datatype_definition.is_pointer) || (0 != strcmp(node->info.datatype.name, "void"))) {
      printf("    %s", node->info.datatype.name);
      for (i = 0; i < node->info.datatype.datatype_definition.is_pointer; i++) {
        printf("*");
      }
      printf(" retval;\n");
    }
    /*
     * func function pointer
     */
    printf("    %s", node->info.datatype.name);
    for (i = 0; i < node->info.datatype.datatype_definition.is_pointer; i++) {
      printf("*");
    }

    printf(" (*func)(");
    if (NULL == node->info.argument_list.first) {
      printf("void");
    }
    for (anode = node->info.argument_list.first; NULL != anode; anode = anode->next) {
      int i;
      if (anode->info.datatype.datatype_definition.is_variadic) {
        printf("...");
        break;
      }
      if (anode->info.datatype.datatype_definition.is_const) {
        printf("const ");
      }
      printf("%s", anode->info.datatype.name);
      for (i = 0; i < anode->info.datatype.datatype_definition.is_pointer; i++) {
        printf("*");
      }

      printf(" /* %s */", anode->info.name);

      if (NULL != anode->next) {
        printf(", ");
      }
    }
    printf(");\n");

    /*
     * argument storage
     */
    if (node->info.argument_list.first) {
      printf("    struct {\n");
      for (anode = node->info.argument_list.first; NULL != anode; anode = anode->next) {
        int i;
        if (anode->info.datatype.datatype_definition.is_variadic) {
          printf("      /* Variadic arguments not supported */\n");
          break;
        }
        printf("      %s", anode->info.datatype.name);
        for (i = 0; i < anode->info.datatype.datatype_definition.is_pointer; i++) {
          printf("*");
        }
        printf(" arg%d; /* %s */\n", cnt++, anode->info.name);
      }
      printf("    } args;\n");
    }
    printf("  } %s;\n\n", node->info.symbol);
  }
  printf("};\n");
  /*
  printf("typedef struct __tarsio_data_s __tarsio_data_t;\n\n");
  */

  for (node = list->first; NULL != node; node = node->next) {
    generate_prototype(node, "extern ", "", ";");
  }
  printf("#endif\n");
}


/****************************************************************************
 * Program
 */
int main(int argc, char* argv[])
{
  int retval = EXIT_SUCCESS;
  tsg_options_t options;
  file_t test_file = FILE_EMPTY;
  prototype_list_t prototype_list = PROTOTYPE_LIST_EMPTY;
  cpp_list_t cpp_list = CPP_LIST_EMPTY;
  unsigned char* buf = NULL;

  /*
   * Handle arguments passed to the program.
   */
  if (0 != tsg_options_init(&options, argc, argv)) {
    retval = EXIT_FAILURE;
    goto options_init_failed;
  }

  /*
   * Read the pre-processed version of the design under test
   */
  if (0 != file_init(&test_file, options.test_filename)) {
    retval = EXIT_FAILURE;
    goto read_preprocessed_file_failed;
  }

  /*
   * Find all pre-processor directives in the test suite.
   */
  if (0 != cpp_list_init(&cpp_list, &test_file)) {
    retval = EXIT_FAILURE;
    goto cpp_list_init_failed;
  }

  buf = reload_symbol_cache(&prototype_list, options.cache_filename);

  generate_struct(&prototype_list, &cpp_list);
  cpp_list_cleanup(&cpp_list);
 cpp_list_init_failed:
  file_cleanup(&test_file);
 read_preprocessed_file_failed:
 options_init_failed:
  if (NULL != buf) {
    free(buf);
  }
  return retval;
}
