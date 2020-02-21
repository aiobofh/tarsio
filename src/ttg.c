/*
 * Tarsio Test-runner Generator
 *
 *              _______          _____ ___        ______
 *                 |      ||    |         |    | |      |
 *                 |      ||    |         |    | |      |
 *                 |   ___||___ |         |___ | |______|
 *
 *              Copyright (C) 2020 AiO Secure Teletronics
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

#include "file.h"
#include "testcase.h"

#include "version.h"

static char field[] = "$Id: ttg,v " VERSION " " __DATE__ " " __TIME__ " " AUTHOR " Exp $";
static char version[] = VERSION;
static char timestamp[] = __DATE__ " " __TIME__;

/****************************************************************************
 * Program usage
 */
static void usage(const char* program_name)
{
  printf("USAGE: %s [-n|--no-module] <test-file> <tarsio-pre-processed-source>\n", program_name);
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
  char* testcases_filename;
  char* header_filename;
  int no_module;
};
typedef struct ttg_options_s ttg_options_t;

static int ttg_options_init(ttg_options_t* options, int argc, char* argv[])
{
  options->no_module = 0;

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
    if (argc == 4) {
      if ((0 == strcmp("-n", argv[1])) || (0 == strcmp("--no-module", argv[1])) || (0 == strcmp("NOMODULE", argv[1]))) {
        options->no_module = 1;
      }
    }
    error1("ERROR: Illegal number (%d) of arguments", argc);
    usage(argv[0]);
    return -1;
  }

  options->testcases_filename = argv[argc - 2];
  options->header_filename = argv[argc - 1];

  return 0;
}

static void generate_tarsio_stats(void) {
  puts("struct __tarsio_stats_s {\n"
       "  size_t success;\n"
       "  size_t fail;\n"
       "  size_t error;\n"
       "  size_t skip;\n"
       "  struct {\n"
       "    size_t pass;\n"
       "    size_t fail;\n"
       "    size_t skip;\n"
       "  } total;\n"
       "};\n"
       "typedef struct __tarsio_stats_s __tarsio_stats_t;\n"
       "__tarsio_stats_t __tarsio_stats;\n");
}

static void generate_tarsio_verdict(void) {
  puts("enum __tarsio_verdict_e {\n"
       "  PASS = 0,\n"
       "  FAIL = 1,\n"
       "  SKIP = 2\n"
       "};"
       "typedef enum __tarsio_verdict_e __tarsio_verdict_t;\n");
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


static void generate_tarsio_result_node(void) {
  puts("struct __tarsio_result_node_s {\n"
       "  struct __tarsio_result_node_s* next;\n"
       "  char* testcase_name;\n"
       "  char* help;\n"
       "  __tarsio_verdict_t verdict;\n"
       "};\n"
       "typedef struct __tarsio_result_node_s __tarsio_result_node_t;\n");
}

static void generate_tarsio_result_list(void) {
  puts("struct __tarsio_result_list_s {\n"
       "  __tarsio_result_node_t* first;\n"
       "  __tarsio_result_node_t* last;\n"
       "};\n"
       "typedef struct __tarsio_result_list_s __tarsio_result_list_t;\n"
       "__tarsio_result_list_t __tarsio_result_list;\n");
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

static void generate_tarsio_append_result(void) {
  puts("void __tarsio_append_result(__tarsio_result_list_t* list, const char* testcase_name, const char* help, __tarsio_verdict_t verdict) {\n"
       "  __tarsio_result_node_t* node = malloc(sizeof(*node));\n"
       "\n"
       "  if (NULL == node) {\n"
       "    fprintf(stderr, \"FATAL: Out of memory while allocating verdict information in Tarsio\\n\");\n"
       "    exit(EXIT_FAILURE);\n"
       "  }\n"
       "\n"
       "  node->testcase_name = (char*)testcase_name;\n"
       "  node->help = (char*)help;\n"
       "  node->verdict = verdict;\n"
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
       "  __tarsio_stats.success = __tarsio_stats.fail = __tarsio_stats.error = __tarsio_stats.skip = __tarsio_stats.total.pass = __tarsio_stats.total.fail = __tarsio_stats.total.skip = 0;\n"
       "  __tarsio_failure_list.first = __tarsio_failure_list.last = NULL;\n"
       "  __tarsio_result_list.first = __tarsio_result_list.last = NULL;\n"
       "  __tarsio_options.compact = __tarsio_options.verbose = __tarsio_options.xml_output = 0;\n"
       "}\n");
}



#ifdef SASC
#define AMIGA
#endif
#ifdef VBCC
#define AMIGA
#endif

#ifndef AMIGA
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
       "    if (0 == strcmp(argv[i], \"-x\")) {\n"
       "      __tarsio_options.xml_output = 1;\n"
       "    }\n"
       "  }\n"
       "}\n");
}
#else
static void generate_tarsio_handle_arguments(void) {
  puts("void __tarsio_handle_arguments(int argc, char* argv[]) {\n"
       "  int i;\n"
       "  for (i = 1; i < argc; i++) {\n"
       "    if (0 == strcmp(argv[i], \"COMPACT\")) {\n"
       "      __tarsio_options.compact = 1;\n"
       "    }\n"
       "    if (0 == strcmp(argv[i], \"VERBOSE\")) {\n"
       "      __tarsio_options.verbose = 1;\n"
       "    }\n"
       "    if (0 == strcmp(argv[i], \"XML\")) {\n"
       "      __tarsio_options.xml_output = 1;\n"
       "    }\n"
       "  }\n"
       "}\n");
}
#endif

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
       "  size_t i = 0;\n"
       "  char* ptr = (char*)__tarsio_mock_data;\n"
       "  while (i < mock_data_size) {\n"
       "    ptr[i++] = 0;\n"
       "  }\n"
       "  func(__tarsio_mock_data, name);\n"
       "  if (1 == __tarsio_options.verbose) {\n"
       "    if (skip != __tarsio_stats.skip) {\n"
       "      if (0 == __tarsio_options.xml_output) {\n"
       "        printf(\"[SKIP] %s\\n\", name);\n"
       "      }\n"
       "      __tarsio_append_result(&__tarsio_result_list, name, \"\", SKIP);\n"
       "    }\n"
       "    else if (error != __tarsio_stats.error) {\n"
       "      if (0 == __tarsio_options.xml_output) {\n"
       "        printf(\"[ERROR] %s\\n\", name);\n"
       "      }\n"
       "    }\n"
       "    else if (fail != __tarsio_stats.fail) {\n"
       "      if (0 == __tarsio_options.xml_output) {\n"
       "        printf(\"[FAIL] %s\\n\", name);\n"
       "      }\n"
       "      __tarsio_stats.total.fail++;\n"
       "      __tarsio_append_result(&__tarsio_result_list, name, \"\", FAIL);\n"
       "    }\n"
       "    else {\n"
       "      if (0 == __tarsio_options.xml_output) {\n"
       "        printf(\"[PASS] %s\\n\", name);\n"
       "      }\n"
       "      __tarsio_stats.total.pass++;\n"
       "      __tarsio_append_result(&__tarsio_result_list, name, \"\", PASS);\n"
       "    }\n"
       "  }\n"
       "  else {\n"
       "    if (skip != __tarsio_stats.skip) {\n"
       "      if (0 == __tarsio_options.xml_output) {\n"
       "        putc('S', stdout);\n"
       "      }\n"
       "      __tarsio_stats.total.skip++;\n"
       "      __tarsio_append_result(&__tarsio_result_list, name, \"\", SKIP);\n"
       "    }\n"
       "    else if (error != __tarsio_stats.error) {\n"
       "      if (0 == __tarsio_options.xml_output) {\n"
       "        putc('E', stdout);\n"
       "      }\n"
       "    }\n"
       "    else if (fail != __tarsio_stats.fail) {\n"
       "      if (0 == __tarsio_options.xml_output) {\n"
       "        putc('F', stdout);\n"
       "      }\n"
       "      __tarsio_stats.total.fail++;\n"
       "      __tarsio_append_result(&__tarsio_result_list, name, \"\", FAIL);\n"
       "    }\n"
       "    else {\n"
       "      if (0 == __tarsio_options.xml_output) {\n"
       "        putc('.', stdout);\n"
       "      }\n"
       "      __tarsio_stats.total.pass++;\n"
       "      __tarsio_append_result(&__tarsio_result_list, name, \"\", PASS);\n"
       "    }\n"
       "  }\n"
       "}\n");
}

static void generate_tarsio_module_test_execute(void) {
  puts("void __tarsio_module_test_execute(__tarsio_data_t* __tarsio_mock_data, int (*func)(void* __tarsio_mock_data, const char* __tarsio_test_name), const char* name, size_t mock_data_size) {\n"
       "  size_t skip = __tarsio_stats.skip;\n"
       "  size_t fail = __tarsio_stats.fail;\n"
       "  size_t error = __tarsio_stats.error;\n"
       "  size_t i = 0;\n"
       "  char* ptr = (char*)__tarsio_mock_data;\n"
       "  while (i < mock_data_size) {\n"
       "    ptr[i++] = 0;\n"
       "  }\n"
       "  TARSIO_DEFAULT_FUNCS\n"
       "  func(__tarsio_mock_data, name);\n"
       "  if (1 == __tarsio_options.verbose) {\n"
       "    if (skip != __tarsio_stats.skip) {\n"
       "      if (0 == __tarsio_options.xml_output) {\n"
       "        printf(\"[SKIP] %s\\n\", name);\n"
       "      }\n"
       "      __tarsio_stats.total.skip++;\n"
       "      __tarsio_append_result(&__tarsio_result_list, name, \"\", SKIP);\n"
       "    }\n"
       "    else if (error != __tarsio_stats.error) {\n"
       "      if (0 == __tarsio_options.xml_output) {\n"
       "        printf(\"[ERROR] %s\\n\", name);\n"
       "      }\n"
       "    }\n"
       "    else if (fail != __tarsio_stats.fail) {\n"
       "      if (0 == __tarsio_options.xml_output) {\n"
       "        printf(\"[FAIL] %s\\n\", name);\n"
       "      }\n"
       "      __tarsio_stats.total.fail++;\n"
       "      __tarsio_append_result(&__tarsio_result_list, name, \"\", FAIL);\n"
       "    }\n"
       "    else {\n"
       "      printf(\"[PASS] %s\\n\", name);\n"
       "      __tarsio_stats.total.pass++;\n"
       "      __tarsio_append_result(&__tarsio_result_list, name, \"\", PASS);\n"
       "    }\n"
       "  }\n"
       "  else {\n"
       "    if (skip != __tarsio_stats.skip) {\n"
       "      if (0 == __tarsio_options.xml_output) {\n"
       "        putc('S', stdout);\n"
       "      }\n"
       "      __tarsio_stats.total.skip++;\n"
       "      __tarsio_append_result(&__tarsio_result_list, name, \"\", SKIP);\n"
       "    }\n"
       "    else if (error != __tarsio_stats.error) {\n"
       "      putc('E', stdout);\n"
       "    }\n"
       "    else if (fail != __tarsio_stats.fail) {\n"
       "      if (0 == __tarsio_options.xml_output) {\n"
       "        putc('F', stdout);\n"
       "      }\n"
       "      __tarsio_stats.total.fail++;\n"
       "      __tarsio_append_result(&__tarsio_result_list, name, \"\", FAIL);\n"
       "    }\n"
       "    else {\n"
       "      if (0 == __tarsio_options.xml_output) {\n"
       "        putc('.', stdout);\n"
       "      }\n"
       "      __tarsio_stats.total.pass++;\n"
       "      __tarsio_append_result(&__tarsio_result_list, name, \"\", PASS);\n"
       "    }\n"
       "  }\n"
       "}\n");
}

static void gemerate_tarsio_xml_output(void) {
  puts("int __tarsio_xml_output(const char* file_name, const char* dut) {\n"
       "\n"
       "  const time_t current_time = time(NULL);\n"
       "  char timestamp[40];\n"
       "  struct tm tm;\n"
       "  __tarsio_result_node_t* node;\n"
       "  __tarsio_failure_node_t* fnode;\n"
       "  const size_t total = __tarsio_stats.total.fail + __tarsio_stats.total.pass + __tarsio_stats.total.skip;\n"
       "\n"
       "  localtime_r(&current_time, &tm);\n"
       "  strftime(timestamp, sizeof(timestamp), \"%Y-%m-%dT%H:%M:%S\", &tm);\n"
       "\n"
       "  printf(\"<?xml version \\\"1.0\\\" encoding=\\\"UTF-8\\\"?>\\n\"\n"
       "         \"<testsuites>\\n\"\n"
       "         \"  <testsuite name=\\\"%s\\\"\\n\"\n"
       "         \"             errors=\\\"%d\\\"\\n\"\n"
       "         \"             tests=\\\"%lu\\\"\\n\"\n"
       "         \"             failures=\\\"%lu\\\"\\n\"\n"
       "         \"             skipped=\\\"%lu\\\"\\n\"\n"
       "         \"             timestamp=\\\"%s\\\">\\n\",\n"
       "         file_name,\n"
       "         0,\n"
       "         total,\n"
       "         __tarsio_stats.total.fail,\n"
       "         __tarsio_stats.total.skip,\n"
       "         timestamp);\n"
       "\n"
       "  for (node = __tarsio_result_list.first; NULL != node; node = node->next) {\n"
       "    printf(\"    <testcase classname=\\\"%s\\\" name=\\\"%s\\\">\\n\", file_name, node->testcase_name);\n"
       "    switch(node->verdict) {\n"
       "    case SKIP:\n"
       "      puts(\"      <skipped/>\");\n"
       "      break;\n"
       "    case FAIL:\n"
       "      puts(\"      <failure message=\\\"test failure\\\">\");"
       "      for (fnode = __tarsio_failure_list.first; NULL != fnode; fnode = fnode->next) {\n"
       "        if (0 == strcmp(fnode->testcase_name, node->testcase_name)) {\n"
       "          fprintf(stderr, \"  %s:%lu:\\n\", fnode->file, (unsigned long int)fnode->line);\n"
       "          fprintf(stderr, \"    %s\\n\", fnode->help);\n"
       "        }\n"
       "      }\n"
       "      puts(\"      </failure>\");\n"
       "      break;\n"
       "    default:\n"
       "      break;\n"
       "    }\n"
       "    puts(\"    </testcase>\");\n"
       "  }\n"
       "\n"
       "  puts(\"  </testsuite>\\n\"\n"
       "       \"</testsuites>\");\n"
       "  return 0;\n"
       "}\n"
       );
}

static void generate_tarsio_summary(void) {
  puts("int __tarsio_summary(void) {\n"
       "  int retval = 0;\n"
       "  int fail_cnt = 0;\n"
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
       "    fprintf(stderr, \"  %s:%lu:\\n\", fnode->file, (unsigned long int)fnode->line);\n"
       "    fprintf(stderr, \"    %s\\n\", fnode->help);\n"
       "    last_test_name = fnode->testcase_name;\n"
       "    fail_cnt++;\n"
       "  }\n"
       "\n"
       "  if (1 == __tarsio_options.verbose) {\n"
       "    fprintf(stdout, \"%lu success \", (unsigned long int)__tarsio_stats.total.pass);\n"
       "    fprintf(stdout, \"%lu failures \", (unsigned long int)__tarsio_stats.total.fail);\n"
       "    fprintf(stdout, \"%lu skipped \", (unsigned long int)__tarsio_stats.total.skip);\n"
       "    fprintf(stdout, \"\\n\");"
       "  }\n"
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
       "  __tarsio_result_node_t* rnode = __tarsio_result_list.first;\n"
       "  while (NULL != rnode) {\n"
       "    __tarsio_result_node_t* next_node = rnode->next;\n"
       "    free(rnode);\n"
       "    rnode = next_node;\n"
       "  }\n"
       "  while (NULL != fnode) {\n"
       "    __tarsio_failure_node_t* next_node = fnode->next;\n"
       "    free(fnode);\n"
       "    fnode = next_node;\n"
       "  }\n"
       "}\n");
}

static int generate_test_runner(testcase_list_t* list, const char* file, int no_module) {
  int retval = 0;
  testcase_node_t* node;
  const char* data_file = file;
  char* ptr;

  printf("#include <stdio.h>\n");
  printf("#include <time.h>\n");
  printf("#include <stdlib.h>\n\n");
  printf("#include \"tarsio.h\"\n");
  puts("extern struct tm *localtime_r (const time_t *__restrict __timer, struct tm *__restrict __tp);");
  while (NULL != (ptr = strstr(data_file, ":"))) {
    data_file = ptr + 1;
  }
  while (NULL != (ptr = strstr(data_file, "/"))) {
    data_file = ptr + 1;
  }
  while (NULL != (ptr = strstr(data_file, "\\"))) {
    data_file = ptr + 1;
  }

  printf("#include \"%s\"\n", data_file);

  for (node = list->first; NULL != node; node = node->next) {
    printf("extern int __%s(void*, const char*);\n", node->name);
  }

  /*
   * This is actually the stupidest, but given portability the smartest way
   */
  generate_tarsio_stats();
  generate_tarsio_verdict();
  generate_tarsio_options();
  generate_tarsio_result_node();
  generate_tarsio_result_list();
  generate_tarsio_failure_node();
  generate_tarsio_failure_list();
  generate_tarsio_append_result();
  generate_tarsio_append_failure();
  generate_tarsio_assert_eq();
  generate_tarsio_init();
  generate_tarsio_handle_arguments();
  generate_tarsio_skip();
  generate_tarsio_unit_test_execute();
  if (0 == no_module) {
    generate_tarsio_module_test_execute();
  }
  gemerate_tarsio_xml_output();
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
  if (0 != generate_test_runner(&testcase_list, options.header_filename, options.no_module)) {
    retval = EXIT_FAILURE;
    goto generate_test_runner_failed;
  }

 testcase_list_init_failed:
  file_cleanup(&test_file);
 generate_test_runner_failed:
  testcase_list_cleanup(&testcase_list);
 read_test_file_failed:
options_init_failed:
  return retval;
}
