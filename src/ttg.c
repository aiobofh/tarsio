/*
 * Tarsio Test-runner Generator
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
#include "testcase.h"

/****************************************************************************
 * Program usage
 */
static void usage(const char* program_name)
{
  printf("USAGE: %s <test-file> <tarsio-pre-processed-source>\n", program_name);
}

/****************************************************************************
 * Option handling
 */
struct ttg_options_s {
  char* testcases_filename;
  char* header_filename;
};
typedef struct ttg_options_s ttg_options_t;

static int ttg_options_init(ttg_options_t* options, int argc, char* argv[])
{
  if (argc != 3) {
    error1("ERROR: Illegal number (%d) of arguments", argc);
    usage(argv[0]);
    return -1;
  }

  options->testcases_filename = argv[1];
  options->header_filename = argv[2];

  return 0;
}

static int generate_test_runner(testcase_list_t* list, const char* file) {
  int retval = 0;
  testcase_node_t* node = NULL;

  printf("#include <stdio.h>\n\n");
  printf("#include <tarsio.h>\n");

  printf("#include \"%s\"\n", file);

  for (node = list->first; NULL != node; node = node->next) {
    printf("extern int __tarsio_test_%s(void*);\n", node->name);
  }
  printf("\n"
         "__tarsio_data_t __tarsio_mock_data;\n");

  printf("\n"
         "int main(int argc, char* argv[]) {\n"
         "  int retval = EXIT_SUCCESS;\n"
         "\n"
         "  __tarsio_handle_arguments(argc, argv);\n");
  for (node = list->first; NULL != node; node = node->next) {
    switch (node->type) {
    case TESTCASE_IS_UNIT_TEST:
      printf("  __tarsio_unit_test_execute(&__tarsio_mock_data, __tarsio_test_%s, \"%s\", sizeof(__tarsio_mock_data));\n", node->name, node->name);
      break;
    case TESTCASE_IS_MODULE_TEST:
      printf("  __tarsio_module_test_execute(&__tarsio_mock_data, __tarsio_test_%s, \"%s\", sizeof(__tarsio_mock_data));\n", node->name, node->name);
      break;
    case TESTCASE_IS_UNKNOWN:
      error0("This should never happen :)");
      break;
    }
  }
  printf("  return retval;\n"
         "}\n");

  return retval;
}

int main(int argc, char* argv[]) {
  int retval = EXIT_SUCCESS;
  testcase_list_t testcase_list;
  file_t test_file;
  ttg_options_t options;

  memset(&testcase_list, 0, sizeof(testcase_list));
  memset(&test_file, 0, sizeof(test_file));

  /*
   * Handle arguments passed to the program.
   */
  if (0 != ttg_options_init(&options, argc, argv)) {
    retval = EXIT_FAILURE;
    goto options_init_failed;
  }

  /*
   * Read the test-suite C-file.
   */
  if (0 != file_init(&test_file, options.testcases_filename)) {
    retval = EXIT_FAILURE;
    goto read_test_file_failed;
  }

  /*
   * Extract test-cases
   */
  if (0 != testcase_list_init(&testcase_list, &test_file)) {
    retval = EXIT_FAILURE;
    goto testcase_list_init_failed;
  }

  /*
   * Generate test-runner
   */
  if (0 != generate_test_runner(&testcase_list, options.header_filename)) {
    retval = EXIT_FAILURE;
    goto generate_test_runner_failed;
  }

 testcase_list_init_failed:
 generate_test_runner_failed:
  testcase_list_cleanup(&testcase_list);
 read_test_file_failed:
options_init_failed:
  return retval;
}
