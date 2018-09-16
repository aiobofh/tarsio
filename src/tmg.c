/*
 * Tarsio Mock Generator
 *
 *              _______          _____ ___        ______
 *                 |      ||    |         |    | |      |
 *                 |      ||    |         |    | |      |
 *                 |   ___||___ |         |___ | |______|
 *
 *                   Copyleft AiO Secure Teletronics
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"

#include "prototype.h"
#include "symbol_cache.h"

/****************************************************************************
 * Program usage
 */
static void usage(const char* program_name)
{
  printf("USAGE: %s <cache-file>\n", program_name);
}

/****************************************************************************
 * Option handling
 */
struct tmg_options_s {
  char* cache_filename;
  char* header_filename;
};
typedef struct tmg_options_s tmg_options_t;

static int tmg_options_init(tmg_options_t* options, int argc, char* argv[])
{
  if (argc != 3) {
    error1("ERROR: Illegal number (%d) of arguments", argc);
    usage(argv[0]);
    return -1;
  }

  options->cache_filename = argv[1];
  options->header_filename = argv[2];

  return 0;
}

/*
 * TODO: Refactor this to write to disk more efficient
 */
static void generate_proxies(prototype_list_t* list, const char* file) {
  prototype_node_t* node = NULL;

  printf("/*\n"
         " * Tarsio mock-up functions - This file is generated by tmg\n"
         " *\n"
         " *              _______          _____ ___        ______\n"
         " *                 |      ||    |         |    | |      |\n"
         " *                 |      ||    |         |    | |      |\n"
         " *                 |   ___||___ |         |___ | |______|\n"
         " *\n"
         " *                   Copyleft AiO Secure Teletronics\n"
         " *\n");
  printf(" * This is the proxy function implementations for all used functions\n"
         " * in the design under test. They should be stored into a .c file to\n"
         " * be compiled to an object file and later linked with the object file\n"
         " * compiled from the modified pre-processed design under test, and the\n"
         " * generated test-runner code.\n"
         " */\n\n");

  printf("#include \"%s\"\n\n", file);

  printf("extern __tarsio_data_t __tarsio_mock_data;\n\n");

  for (node = list->first; NULL != node; node = node->next) {

    argument_node_t* anode = NULL;
    int idx = 0;
    generate_prototype(node, "", "__tarsio_proxy_", " {");

    for (anode = node->info.argument_list.first; NULL != anode; anode = anode->next) {
      if (anode->info.datatype.datatype_definition.is_variadic) {
        printf("  /* Variadic arguments not supported */\n");
        break;
      }
      printf("  __tarsio_mock_data.%s.args.arg%d = ", node->info.symbol, idx);
      if (anode->info.datatype.datatype_definition.is_const) {
        int i = 0;
        printf("(%s", anode->info.datatype.name);
        for (i = 0; i < anode->info.datatype.datatype_definition.is_pointer; i++) {
          printf("*");
        }
        printf(")");
      }
      if (anode->info.name) {
        printf("%s", anode->info.name);
      }
      printf(";\n");
      idx++;
    }

    printf("  __tarsio_mock_data.%s.call_count++;\n", node->info.symbol);

    printf("  if (NULL != __tarsio_mock_data.%s.func) {\n", node->info.symbol);
    if ((0 != strcmp("void", node->info.datatype.name)) || (0 != node->info.datatype.datatype_definition.is_pointer)) {
      printf("    return ");
    }
    else {
      printf("    ");
    }

    printf("__tarsio_mock_data.%s.func(", node->info.symbol);
    for (anode = node->info.argument_list.first; NULL != anode; anode = anode->next) {
      if (anode->info.datatype.datatype_definition.is_variadic) {
        printf("/* Variadic arguments not supported */ 0x0");
        break;
      }
      printf("%s", anode->info.name);
      if (NULL != anode->next) {
        printf(", ");
      }
    }
    printf(");\n");
    printf("  }\n");

    if ((0 != strcmp("void", node->info.datatype.name)) || (0 != node->info.datatype.datatype_definition.is_pointer)) {
      printf("  return __tarsio_mock_data.%s.retval;\n", node->info.symbol);
    }

    printf("}\n\n");
  }

  printf("/*\n"
         " * This function is only generated to reduce warnings about unused\n"
         " * funcitons. :)\n"
         " */\n"
         "typedef void (*__tarsio_fake_function_pointer)();\n"
         "void __tarsio_make_fake_dependency_to_compile_functions() {\n"
         "  __tarsio_fake_function_pointer func = NULL;\n");
  for (node = list->first; NULL != node; node = node->next) {
    printf("  func = (__tarsio_fake_function_pointer)__tarsio_proxy_%s;\n", node->info.symbol);
    printf("  func();\n");
  }
  printf("}\n");
}

/****************************************************************************
 * Program
 */
int main(int argc, char* argv[])
{
  int retval = EXIT_SUCCESS;
  tmg_options_t options;
  prototype_list_t prototype_list = PROTOTYPE_LIST_EMPTY;

  /*
   * Handle arguments passed to the program.
   */
  if (0 != tmg_options_init(&options, argc, argv)) {
    retval = EXIT_FAILURE;
    goto options_init_failed;
  }

  reload_symbol_cache(&prototype_list, options.cache_filename);

  generate_proxies(&prototype_list, options.header_filename);

 options_init_failed:
  return retval;
}
