/*
 * Tarsio Auto Mocker
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

#include "file.h"
#include "prototype.h"
#include "symbol_cache.h"

/****************************************************************************
 * Program usage
 */
static void usage(const char* program_name)
{
  printf("USAGE: %s <cache-file> <pre-processed-source>\n", program_name);
}

/****************************************************************************
 * Option handling
 */
struct tam_options_s {
  char* cache_filename;
  char* source_filename;
};
typedef struct tam_options_s tam_options_t;

static int tam_options_init(tam_options_t* options, int argc, char* argv[])
{
  if (argc != 3) {
    error1("ERROR: Illegal number (%d) of arguments", argc);
    usage(argv[0]);
    return -1;
  }

  options->cache_filename = argv[1];
  options->source_filename = argv[2];

  return 0;
}

size_t first_line(prototype_list_t* list) {
  prototype_node_t* node = NULL;
  for (node = list->first; NULL != node; node = node->next) {
    if (node->info.is_function_implementation) {
      return node->info.is_function_implementation;
    }
  }
  return 0;
}

static void generate_extern_proxy_prototypes(prototype_list_t* list) {
  prototype_node_t* node = NULL;
  printf("/*\n"
         " * This external declaration part is inserted into the pre-processed\n"
         " * right before the first actual function definition.\n"
         " */\n");
  for (node = list->first; NULL != node; node = node->next) {
    generate_prototype(node, "extern ", "__tarsio_proxy_", ";");
  }
  printf("\n");
}

/*
 * TODO: Refactor this to read larger chunks of the memory and write them
 *       to disk. E.g. by writing every byte until next offset (sorted).
 */
static void generate_proxified(prototype_list_t* list, file_t* file) {
  size_t offset = 0;
  size_t line = 0;
  size_t first_implementation_line = first_line(list);

  /*
   * This should be done smarter... Like a sorted list of all available offsets
   */
  while (offset < file->len) {
    prototype_node_t* node = NULL;
    char* symbol = NULL;
    char c = file->buf[offset];
    if (line == first_implementation_line) {
      generate_extern_proxy_prototypes(list);
      first_implementation_line = 0;
    }
    for (node = list->first; NULL != node; node = node->next) {
      symbol_usage_node_t* snode = NULL;
      for (snode = node->info.symbol_usage_list.first; NULL != snode; snode = snode->next) {
        if (offset == snode->info.offset - strlen(node->info.symbol)) {
          symbol = node->info.symbol;
        }
      }
    }
    if (symbol) {
      printf("__tarsio_proxy_%s", symbol);
      offset += strlen(symbol) - 1;
    }
    else {
      putc(c, stdout);
    }
    line += ('\n' == c);
    offset++;
  }
}

/****************************************************************************
 * Program
 */
int main(int argc, char* argv[])
{
  int retval = EXIT_SUCCESS;
  tam_options_t options;
  file_t source_file = FILE_EMPTY;
  prototype_list_t prototype_list = PROTOTYPE_LIST_EMPTY;

  /*
   * Handle arguments passed to the program.
   */
  if (0 != tam_options_init(&options, argc, argv)) {
    retval = EXIT_FAILURE;
    goto options_init_failed;
  }

  /*
   * Read the pre-processed version of the design under test
   */
  if (0 != file_init(&source_file, options.source_filename)) {
    retval = EXIT_FAILURE;
    goto read_preprocessed_file_failed;
  }

  reload_symbol_cache(&prototype_list, options.cache_filename);

  /*
   * Generate a new version of the pre-processed input but with extern
   * declarations to all used function proxies, and replaced function
   * usage.
   */
  generate_proxified(&prototype_list, &source_file);

 read_preprocessed_file_failed:
 options_init_failed:
  return retval;
}
