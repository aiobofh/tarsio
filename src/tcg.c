/*
 * Tarsio symbol Cache Generator
 *
 *              _______          _____ ___        ______
 *                 |      ||    |         |    | |      |
 *                 |      ||    |         |    | |      |
 *                 |   ___||___ |         |___ | |______|
 *
 *                   Copyleft AiO Secure Teletronics
 *
 * This program parse a pre-processed C file and create a binary file with
 * the required information for the Tarsio Mock Generator and Tarsio Struct
 * Generator tools.
 *
 * The idea behind this tool is to make every part of your build as tiny and
 * stand-alone as possible. But also fast.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "error.h"

#include "file.h"
#include "prototype.h"
#include "symbol_cache.h"

#include "version.h"

static char field[] = "$Id: tcg,v " VERSION " " __DATE__ " " __TIME__ " " AUTHOR " Exp $";
static char version[] = VERSION;
static char timestamp[] = __DATE__ " " __TIME__;

/****************************************************************************
 * Program usage
 */
static void usage(const char* program_name)
{
  printf("USAGE: %s <pre-processed-source> <output>\n", program_name);
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
struct tcg_options_s {
  char* code_filename;
  char* output_filename;
};
typedef struct tcg_options_s tcg_options_t;

static int tcg_options_init(tcg_options_t* options, int argc, char* argv[])
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

  options->code_filename = argv[1];
  options->output_filename = argv[2];

  return 0;
}

/****************************************************************************
 * Serialization
 */
static int symbol_usage_serialize(FILE* fd, symbol_usage_node_t* node)
{
  const size_t s = sizeof(*node);

  /*
   * Write the symbol usage node struct
   */
  if (1 != fwrite(node, s, 1, fd)) {
    return -1;
  }
  return 0;
}

static int argument_serialize(FILE* fd, argument_node_t* node)
{
  const size_t s = sizeof(*node);

  /*
   * Write the argument node struct
   */
  if (1 != fwrite(node, s, 1, fd)) {
    return -1;
  }
  return 0;
}

static int argument_type_and_name_serialize(FILE* fd, argument_node_t* node)
{
  datatype_t* dt = &node->info.datatype;

  /*
   * Write the argument name string
   */
  if (NULL != node->info.name) {
    if (1 != fwrite(node->info.name, strlen(node->info.name) + 1, 1, fd)) {
      return -2;
    }
  }
  else {
    if (1 != fwrite("", strlen("") + 1, 1, fd)) {
      return -3;
    }
  }

  /*
   * And lastly write the datatype name
   */
  if (NULL != dt->name) {
    if (1 != fwrite(dt->name, strlen(dt->name) + 1, 1, fd)) {
      return -4;
    }
  }
  else {
    if (1 != fwrite("", strlen("") + 1, 1, fd)) {
      return -5;
    }
  }
  return 0;
}

static int return_type_and_symbol_name_serialize(FILE* fd, prototype_node_t* node)
{
  datatype_t* dt = &node->info.datatype;

  /*
   * And then the return datatype name.
   */
  if (1 != fwrite(dt->name, strlen(dt->name) + 1, 1, fd)) {
    return -1;
  }

  /*
   * Then write the symbol name
   */
  if (1 != fwrite(node->info.symbol, strlen(node->info.symbol) + 1, 1, fd)) {
    return -2;
  }
  return 0;
}

static int prototype_serialize(FILE* fd, prototype_node_t* node)
{
  const size_t s = sizeof(*node);
  argument_node_t* an;
  symbol_usage_node_t* un;

  /*
   * Write the prototype node struct
   */
  if (1 != fwrite(node, s, 1, fd)) {
    return -1;
  }

  /*
   * Off to the argument list
   */
  for (an = node->info.argument_list.first; NULL != an; an = an->next) {
    if (0 != argument_serialize(fd, an)) {
      return -4;
    }
  }

  /*
   * And lastly the symbol usage list
   */
  for (un = node->info.symbol_usage_list.first; NULL != un; un = un->next) {
    if (0 != symbol_usage_serialize(fd, un)) {
      return -5;
    }
  }

  return 0;
}

/****************************************************************************
 * Output binary symbol cache
 */
static int generate_symbol_cache(prototype_list_t* list, const char* file)
{
  prototype_node_t* n;
  FILE* fd;

  if (NULL == (fd = fopen(file, "wb"))) {
    return -1;
  }

  fwrite("TCH1", 4, 1, fd);

  if (1 != fwrite(list, sizeof(*list), 1, fd)) {
    return -2;
  }

  for (n = list->first; NULL != n; n = n->next) {
    if (0 != prototype_serialize(fd, n)) {
      return -3;
    }
  }

  for (n = list->first; NULL != n; n = n->next) {
    if (0 != return_type_and_symbol_name_serialize(fd, n)) {
      return -4;
    }
  }

  for (n = list->first; NULL != n; n = n->next) {
    argument_node_t* an;
    for (an = n->info.argument_list.first; NULL != an; an = an->next) {
      if (0 != argument_type_and_name_serialize(fd, an)) {
        return -5;
      }
    }
  }

  fclose(fd);

  return 0;
}

static int compare_prototype_lists(prototype_list_t* l1, prototype_list_t* l2)
{
  prototype_node_t* n1;
  prototype_node_t* n2 = l2->first;
  debug0("Self-diagnostics on symbol list:");
  for (n1 = l1->first; NULL != n1; n1 = n1->next) {
    argument_node_t* a1;
    argument_node_t* a2;
    debug2(" Checking symbol '%s' == '%s'...", n1->info.symbol, n2->info.symbol);
    if (0 != strcmp(n1->info.symbol, n2->info.symbol)) {
      error0("  Checking symbol");
      error2(" '%s' != '%s'", n1->info.symbol, n2->info.symbol);
      return -1;
    }
    debug2(" Checking return type '%s' == '%s'...", n1->info.datatype.name, n2->info.datatype.name);
    if (0 != strcmp(n1->info.datatype.name, n2->info.datatype.name)) {
      return -2;
    }
    a2 = n2->info.argument_list.first;
    for (a1 = n1->info.argument_list.first; NULL != a1; a1 = a1->next) {
      debug2("  Checking argument type '%s' == '%s'...", a1->info.datatype.name, a2->info.datatype.name);
      debug1("  a1 = %p", a1);
      debug1("  a2 = %p", a2);
      if (NULL != a1->info.datatype.name) {
        if (0 != strcmp(a1->info.datatype.name, a2->info.datatype.name)) {
          error1("  Checking argument type 1 for '%s'", n1->info.symbol);
          error2("  '%s' != '%s'...", a1->info.datatype.name, a2->info.datatype.name);
          /* return -3; */
        }
      }
      else {
        if (NULL != a2->info.datatype.name) {
          error1("  Checking argument type 2 for '%s'", n1->info.symbol);
          error2("  '%s' != '%s'...", a1->info.datatype.name, a2->info.datatype.name);
          return -4;
        }
      }
      debug2("  Checking argument name '%s' == '%s'...", a1->info.name, a2->info.name);
      if (NULL != a1->info.name) {
        if (0 != strcmp(a1->info.name, a2->info.name)) {
          error0("  Bouth should be NULL...");
          return -5;
        }
      }
      else {
        if (NULL != a2->info.name) {
          error0("  Both should be NULL...");
          return -6;
        }
      }
      a2 = a2->next;
    }
    n2 = n2->next;
  }
  return 0;
}

/****************************************************************************
 * Program
 */
int main(int argc, char* argv[])
{
  int retval = EXIT_SUCCESS;
  tcg_options_t options;
  file_t code_file = FILE_EMPTY;
  prototype_list_t prototype_list = PROTOTYPE_LIST_EMPTY;
  prototype_list_t test_prototype_list = PROTOTYPE_LIST_EMPTY;

  /*
   * Handle arguments passed to the program.
   */
  if (0 != tcg_options_init(&options, argc, argv)) {
    retval = EXIT_FAILURE;
    goto options_init_failed;
  }

  /*
   * Read the pre-processed version of the design under test
   */
  if (0 != file_init(&code_file, options.code_filename)) {
    retval = EXIT_FAILURE;
    goto read_preprocessed_file_failed;
  }

  /*
   * Find all prototypes to declare
   */
  if (0 != prototype_list_init(&prototype_list, &code_file)) {
    retval = EXIT_FAILURE;
    goto prototypes_init_failed;
  }

  /*
   * Find function calls to any of the prototypes within the design under
   * test to be replaced by calls to the Tarsio proxy functions instead.
   */
  if (0 != prototype_usage(&prototype_list, &code_file)) {
    retval = EXIT_FAILURE;
    goto prototypes_usage_failed;
  }

  /*
   * Remove all unsued prototypes for less looping later on.
   */
  if (0 != prototype_remove_unused(&prototype_list)) {
    retval = EXIT_FAILURE;
    goto prototype_remove_unused_failed;
  }

  /*
   * Extract funciton return data types of all used function.
   */
  if (0 != prototype_extract_return_types(&prototype_list)) {
    retval = EXIT_FAILURE;
    goto prototype_extract_return_type_failed;
  }

  /*
   * Extract function arguments and datatypes of all used functions.
   */
  if (0 != prototype_extract_arguments(&prototype_list)) {
    retval = EXIT_FAILURE;
    goto prototype_extract_arguments_failed;
  }

  if (0 != generate_symbol_cache(&prototype_list, options.output_filename)) {
    retval = EXIT_FAILURE;
    goto generate_symbol_cache_failed;
  }

  /*
   * Self-test - Since this list is the foundation of all the other
   * tools.
   */
  reload_symbol_cache(&test_prototype_list, options.output_filename);

  if (0 != compare_prototype_lists(&prototype_list, &test_prototype_list)) {
    retval = EXIT_FAILURE;
    goto compare_prototype_lists_failed;
  }

 compare_prototype_lists_failed:
 generate_symbol_cache_failed:
 prototype_extract_arguments_failed:
 prototype_extract_return_type_failed:
 prototype_remove_unused_failed:
 prototypes_usage_failed:
  prototype_list_cleanup(&prototype_list);
 prototypes_init_failed:
  file_cleanup(&code_file);
 read_preprocessed_file_failed:
 options_init_failed:
  return retval;
}
