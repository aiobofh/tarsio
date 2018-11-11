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

static void generate_tarsio_stats(void) {
  puts("struct __tarsio_stats_s {\n"
       "  size_t success;\n"
       "  size_t fail;\n"
       "  size_t error;\n"
       "  size_t skip;\n"
       "};\n"
       "typedef struct __tarsio_stats_s __tarsio_stats_t;\n"
       "__tarsio_stats_t __tarsio_stats;\n");
}

static void generate_tarsio_options(void) {
  puts("struct __tarsio_options_s {\n"
       "  int compact;\n"
       "  int verbose;\n"
       "  int xml_output;\n"
       "};\n"
       "typedef struct __tarsio_options_s __tarsio_options_t;\n"
       "__tarsio_options_t __tarsio_options;\n");
}

static void generate_tarsio_failure_node(void) {
  puts("struct __tarsio_failure_node_s {\n"
       "  struct __tarsio_failure_node_s* next;\n"
       "  char* testcase_name;\n"
       "  char* help;\n"
       "  char* file;\n"
       "  size_t line;\n"
       "};\n"
       "typedef struct __tarsio_failure_node_s __tarsio_failure_node_t;\n");
}

static void generate_tarsio_failure_list(void) {
  puts("struct __tarsio_failure_list_s {\n"
       "  __tarsio_failure_node_t* first;\n"
       "  __tarsio_failure_node_t* last;\n"
       "};\n"
       "typedef struct __tarsio_failure_list_s __tarsio_failure_list_t;\n"
       "__tarsio_failure_list_t __tarsio_failure_list;\n");
}

static void generate_tarsio_append_failure(void) {
  puts("void __tarsio_append_failure(__tarsio_failure_list_t* list, const char* testcase_name, const char* help, const char* file, size_t line) {\n"
       "  __tarsio_failure_node_t* node = malloc(sizeof(*node));\n"
       "\n"
       "  if (NULL == node) {\n"
       "    fprintf(stderr, \"FATAL: Out of memory while allocating assert information in Tarsio\\n\");\n"
       "    exit(EXIT_FAILURE);\n"
       "  }\n"
       "\n"
       "  node->testcase_name = (char*)testcase_name;\n"
       "  node->help = (char*)help;\n"
       "  node->file = (char*)file;\n"
       "  node->line = line;\n"
       "  node->next = NULL;\n"
       "\n"
       "  if (NULL == list->first) {\n"
       "    list->first = node;\n"
       "  }\n"
       "  if (NULL != list->last) {\n"
       "    list->last->next = node;\n"
       "  }\n"
       "  list->last = node;\n"
       "}\n");
}

static void generate_tarsio_assert_eq(void) {
  puts("void __tarsio_assert_eq(int res, const char* testcase_name, const char* help, const char* file, size_t line) {\n"
       "  if (res) {\n"
       "    __tarsio_stats.fail++;\n"
       "    __tarsio_append_failure(&__tarsio_failure_list, testcase_name, help, file, line);\n"
       "  }\n"
       "  else {\n"
       "    __tarsio_stats.success++;\n"
       "  }\n"
       "}\n");
}

static void generate_tarsio_init(void) {
  puts("void __tarsio_init(void) {\n"
       "  memset(&__tarsio_stats, 0, sizeof(__tarsio_stats));\n"
       "  memset(&__tarsio_failure_list, 0, sizeof(__tarsio_failure_list));\n"
       "  memset(&__tarsio_options, 0, sizeof(__tarsio_options));\n"
       "}\n");
}

static void generate_tarsio_handle_arguments(void) {
  puts("void __tarsio_handle_arguments(int argc, char* argv[]) {\n"
       "  int i;\n"
       "  for (i = 1; i < argc; i++) {\n"
       "    if (0 == strcmp(argv[i], \"-c\")) {\n"
       "      __tarsio_options.compact = 1;\n"
       "    }\n"
       "    if (0 == strcmp(argv[i], \"-v\")) {\n"
       "      __tarsio_options.verbose = 1;\n"
       "    }\n"
       "    else if (0 == strcmp(argv[i], \"-x\")) {\n"
       "      __tarsio_options.xml_output = 1;\n"
       "    }\n"
       "  }\n"
       "}\n");
}

static void generate_tarsio_skip(void) {
  puts("void __tarsio_skip(const char* reason, const char* test_name) {\n"
       "  __tarsio_stats.skip++;\n"
       "}\n");
}

static void generate_tarsio_unit_test_execute(void) {
  puts("void __tarsio_unit_test_execute(__tarsio_data_t* __tarsio_mock_data, int (*func)(void* __tarsio_mock_data, const char* __tarsio_test_name), const char* name, size_t mock_data_size) {\n"
       "  size_t skip = __tarsio_stats.skip;\n"
       "  size_t fail = __tarsio_stats.fail;\n"
       "  size_t error = __tarsio_stats.error;\n"
       "  memset(__tarsio_mock_data, 0, mock_data_size);\n"
       "  func(__tarsio_mock_data, name);\n"
       "  if (1 == __tarsio_options.verbose) {\n"
       "    if (skip != __tarsio_stats.skip) {\n"
       "      printf(\"[SKIP] %s\\n\", name);\n"
       "    }\n"
       "    else if (error != __tarsio_stats.error) {\n"
       "      printf(\"[ERROR] %s\\n\", name);\n"
       "    }\n"
       "    else if (fail != __tarsio_stats.fail) {\n"
       "      printf(\"[FAIL] %s\\n\", name);\n"
       "    }\n"
       "    else {\n"
       "      printf(\"[PASS] %s\\n\", name);\n"
       "    }\n"
       "  }\n"
       "  else {\n"
       "    if (skip != __tarsio_stats.skip) {\n"
       "      putc('S', stdout);\n"
       "    }\n"
       "    else if (error != __tarsio_stats.error) {\n"
       "      putc('E', stdout);\n"
       "    }\n"
       "    else if (fail != __tarsio_stats.fail) {\n"
       "      putc('F', stdout);\n"
       "    }\n"
       "    else {\n"
       "      putc('.', stdout);\n"
       "    }\n"
       "  }\n"
       "}\n");
}

static void generate_tarsio_summary(void) {
  puts("int __tarsio_summary(void) {\n"
       "  int retval = 0;\n"
       "  __tarsio_failure_node_t* fnode;\n"
       "  char* last_test_name = NULL;\n"
       "  if ((0 == __tarsio_options.compact) ||\n"
       "      ((1 == __tarsio_options.compact) && (NULL != __tarsio_failure_list.first))) {\n"
       "    putc('\\n', stdout);\n"
       "  }\n"
       "\n"
       "  for (fnode = __tarsio_failure_list.first; NULL != fnode; fnode = fnode->next) {\n"
       "    if (((NULL != last_test_name) && (0 != strcmp(last_test_name, fnode->testcase_name))) || (NULL == last_test_name)) {\n"
       "      fprintf(stderr, \"%s:\\n\", fnode->testcase_name);\n"
       "    }\n"
       "    fprintf(stderr, \"  %s:%lu:\\n\", fnode->file, fnode->line);\n"
       "    fprintf(stderr, \"    %s\\n\", fnode->help);\n"
       "    last_test_name = fnode->testcase_name;\n"
       "  }\n"
       "\n"
       "  if (NULL != __tarsio_failure_list.first) {\n"
       "    retval = -1;\n"
       "  }\n"
       "\n"
       "  return retval;\n"
       "}\n");
}

static void generate_tarsio_cleanup(void) {
  puts("void __tarsio_cleanup(void) {\n"
       "  __tarsio_failure_node_t* fnode = __tarsio_failure_list.first;\n"
       "  while (NULL != fnode) {\n"
       "    __tarsio_failure_node_t* next_node = fnode->next;\n"
       "    free(fnode);\n"
       "    fnode = next_node;\n"
       "  }\n"
       "}\n");
}

static int generate_test_runner(testcase_list_t* list, const char* file) {
  int retval = 0;
  testcase_node_t* node;

  printf("#include <stdio.h>\n");
  printf("#include <stdlib.h>\n\n");
  printf("#include \"tarsio.h\"\n");

  printf("#include \"%s\"\n", file);

  for (node = list->first; NULL != node; node = node->next) {
    printf("extern int __%s(void*, const char*);\n", node->name);
  }

  /*
   * This is actually the stupidest, but given portability the smartest way
   */
  generate_tarsio_stats();
  generate_tarsio_options();
  generate_tarsio_failure_node();
  generate_tarsio_failure_list();
  generate_tarsio_append_failure();
  generate_tarsio_assert_eq();
  generate_tarsio_init();
  generate_tarsio_handle_arguments();
  generate_tarsio_skip();
  generate_tarsio_unit_test_execute();
  generate_tarsio_summary();
  generate_tarsio_cleanup();

  /*
   * Done with the silly stuff, on with the test-runner.
   */
  printf("\n"
         "__tarsio_data_t __tarsio_mock_data;\n");

  printf("\n"
         "int main(int argc, char* argv[]) {\n"
         "  int retval;\n"
         "\n"
         "  __tarsio_init();\n"
         "  __tarsio_handle_arguments(argc, argv);\n");
  for (node = list->first; NULL != node; node = node->next) {
    switch (node->type) {
    case TESTCASE_IS_UNIT_TEST:
      printf("  __tarsio_unit_test_execute(&__tarsio_mock_data, __%s, \"%s\", sizeof(__tarsio_mock_data));\n", node->name, node->name);
      break;
    case TESTCASE_IS_MODULE_TEST:
      printf("  __tarsio_module_test_execute(&__tarsio_mock_data, __%s, \"%s\", sizeof(__tarsio_mock_data));\n", node->name, node->name);
      break;
    case TESTCASE_IS_UNKNOWN:
      error0("This should never happen :)");
      break;
    }
  }
  printf("  retval = __tarsio_summary();\n");
  printf("  __tarsio_cleanup();\n");
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
