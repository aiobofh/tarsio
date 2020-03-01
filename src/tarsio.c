/*
 * The Tarsio run-time
 *
 *              _______          _____ ___        ______
 *                 |      ||    |         |    | |      |
 *                 |      ||    |         |    | |      |
 *                 |   ___||___ |         |___ | |______|
 *
 *              Copyright (C) 2020 AiO Secure Teletronics
 *
 * This is the run-time code that will be used to make a generated check-
 * runner execute and evaluate checks independently of each other.
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
 *
 */
#include <time.h>
#include <stdlib.h>
#include <string.h>
#ifndef VBCC
#include <assert.h>
#endif
#include "tarsio.h"

struct __tarsio_stats_s {
  size_t success;
  size_t fail;
  size_t error;
  size_t skip;
  struct {
    size_t pass;
    size_t fail;
    size_t skip;
    size_t error;
  } total;
};
typedef struct __tarsio_stats_s __tarsio_stats_t;
__tarsio_stats_t __tarsio_stats;

enum __tarsio_verdict_e {
  PASS = 0,
  FAIL = 1,
  SKIP = 2,
  ERROR= 3
};typedef enum __tarsio_verdict_e __tarsio_verdict_t;

__tarsio_options_t __tarsio_options;

struct __tarsio_result_node_s {
  struct __tarsio_result_node_s* next;
  char* checkcase_name;
  char* help;
  __tarsio_verdict_t verdict;
};
typedef struct __tarsio_result_node_s __tarsio_result_node_t;

struct __tarsio_result_list_s {
  __tarsio_result_node_t* first;
  __tarsio_result_node_t* last;
};
typedef struct __tarsio_result_list_s __tarsio_result_list_t;
__tarsio_result_list_t __tarsio_result_list;

struct __tarsio_failure_node_s {
  struct __tarsio_failure_node_s* next;
  char* checkcase_name;
  char* help;
  char* file;
  size_t line;
};
typedef struct __tarsio_failure_node_s __tarsio_failure_node_t;

struct __tarsio_failure_list_s {
  __tarsio_failure_node_t* first;
  __tarsio_failure_node_t* last;
};
typedef struct __tarsio_failure_list_s __tarsio_failure_list_t;
__tarsio_failure_list_t __tarsio_failure_list;

extern void __tarsio_setup_mock_functions(void);

void __tarsio_append_result(__tarsio_result_list_t* list, const char* checkcase_name, const char* help, __tarsio_verdict_t verdict) {
  __tarsio_result_node_t* node = malloc(sizeof(*node));

  if (NULL == node) {
    fprintf(stderr, "FATAL: Out of memory while allocating verdict information in Tarsio\n");
    exit(EXIT_FAILURE);
  }

  node->checkcase_name = (char*)checkcase_name;
  node->help = (char*)help;
  node->verdict = verdict;
  node->next = NULL;

  if (NULL == list->first) {
    list->first = node;
  }
  if (NULL != list->last) {
    list->last->next = node;
  }
  list->last = node;
}

void __tarsio_append_failure(__tarsio_failure_list_t* list, const char* checkcase_name, const char* help, const char* file, size_t line) {
  __tarsio_failure_node_t* node = malloc(sizeof(*node));

  if (NULL == node) {
    fprintf(stderr, "FATAL: Out of memory while allocating assert information in Tarsio\n");
    exit(EXIT_FAILURE);
  }

  node->checkcase_name = (char*)checkcase_name;
  node->help = (char*)help;
  node->file = (char*)file;
  node->line = line;
  node->next = NULL;

  if (NULL == list->first) {
    list->first = node;
  }
  if (NULL != list->last) {
    list->last->next = node;
  }
  list->last = node;
}

void __tarsio_assert_eq(int res, const char* checkcase_name, const char* help, const char* file, size_t line) {
  if (res) {
    __tarsio_stats.fail++;
    __tarsio_append_failure(&__tarsio_failure_list, checkcase_name, help, file, line);
  }
  else {
    __tarsio_stats.success++;
  }
}

void __tarsio_init(void) {
  __tarsio_stats.success = __tarsio_stats.fail = __tarsio_stats.error = __tarsio_stats.skip = __tarsio_stats.total.pass = __tarsio_stats.total.fail = __tarsio_stats.total.skip = __tarsio_stats.total.error = 0;
  __tarsio_failure_list.first = __tarsio_failure_list.last = NULL;
  __tarsio_result_list.first = __tarsio_result_list.last = NULL;
  __tarsio_options.compact = __tarsio_options.verbose = __tarsio_options.xml_output = 0;
}

void __tarsio_handle_arguments(int argc, char* argv[]) {
  int i;
  for (i = 1; i < argc; i++) {
    if ((0 == strcmp(argv[i], "-c")) || (0 == strcmp(argv[i], "--compact")) || (0 == strcmp(argv[i], "COMPACT"))) {
      __tarsio_options.compact = 1;
    }
    if ((0 == strcmp(argv[i], "-v")) || (0 == strcmp(argv[i], "--verbose")) || (0 == strcmp(argv[i], "VERBOSE"))) {
      __tarsio_options.verbose = 1;
    }
    if ((0 == strcmp(argv[i], "-x")) || (0 == strcmp(argv[i], "--xml")) || (0 == strcmp(argv[i], "XML"))) {
      __tarsio_options.xml_output = 1;
    }
  }
}

void __tarsio_skip(const char* reason, const char* check_name) {
#ifndef VBCC
  (void)reason; (void)check_name;
#endif
  __tarsio_stats.skip++;
}

static void __tarsio_clear_mock_data(void* __tarsio_mock_data, size_t mock_data_size) {
  size_t i = 0;
  char* ptr = (char*)__tarsio_mock_data;
  while (i < mock_data_size) {
    ptr[i++] = 0;
  }
}

void __tarsio_check_execute(__tarsio_data_t* __tarsio_mock_data,
                            int (*func)(void* __tarsio_mock_data, const char* __tarsio_check_name),
                            const char* name, size_t mock_data_size, int module_check) {
  static char* verdict_str[2][4] = {{".", "F", "S", "E"},
                                    {"[PASS] ", "[FAIL] ", "[SKIP] ", "[ERROR] "}};
  size_t skip = __tarsio_stats.skip;
  size_t fail = __tarsio_stats.fail;
  size_t error = __tarsio_stats.error;
  int is_pass;
  int is_skip;
  int is_fail;
  int is_error;
  __tarsio_verdict_t verdict;
  char* str;

#ifndef VBCC
  assert(((1 == 1) == 1) && ((0 == 1) == 0));
  assert((PASS == 0) && (FAIL == 1) && (SKIP == 2) && (ERROR == 3));
#endif
  __tarsio_clear_mock_data(__tarsio_mock_data, mock_data_size);

#ifndef VC
  if (module_check) {
    /* Call generated inline function in the _data.h file */
    __tarsio_setup_mock_functions();
  }
#else
  (void)module_check;
#endif

  /* Call the check-case */
  func(__tarsio_mock_data, name);

  is_skip = (skip != __tarsio_stats.skip);
  is_fail = (fail != __tarsio_stats.fail);
  is_error = (error != __tarsio_stats.error);
  is_pass = (!is_skip) + (!is_fail) + (!is_error);

  verdict = is_fail + is_skip * 2 + is_error * 3;

  if (0 == __tarsio_options.xml_output) {
    str = verdict_str[__tarsio_options.verbose][verdict];
    printf("%s", str);

    if (1 == __tarsio_options.verbose) {
      puts(name);
    }
  }

  __tarsio_stats.total.pass += is_pass;
  __tarsio_stats.total.skip += is_skip;
  __tarsio_stats.total.fail += is_fail;
  __tarsio_stats.total.error += is_error;

  __tarsio_append_result(&__tarsio_result_list, name, "", verdict);
}

void __tarsio_unit_check_execute(__tarsio_data_t* __tarsio_mock_data, int (*func)(void* __tarsio_mock_data, const char* __tarsio_check_name), const char* name, size_t mock_data_size) {
  __tarsio_check_execute(__tarsio_mock_data, func, name, mock_data_size, 0);
}

void __tarsio_module_check_execute(__tarsio_data_t* __tarsio_mock_data, int (*func)(void* __tarsio_mock_data, const char* __tarsio_check_name), const char* name, size_t mock_data_size) {
  __tarsio_check_execute(__tarsio_mock_data, func, name, mock_data_size, 1);
}

int __tarsio_xml_output(const char* file_name, const char* dut) {

  const time_t current_time = time(NULL);
  char timestamp[40];
  struct tm* tm;
  __tarsio_result_node_t* node;
  __tarsio_failure_node_t* fnode;
  const size_t total = __tarsio_stats.total.fail + __tarsio_stats.total.pass + __tarsio_stats.total.skip;

  tm = localtime(&current_time);
  if (NULL == tm) {
    fprintf(stderr, "ERROR: Could not get localtime\n");
    exit(1);
  }
  strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", tm);

  printf("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
         "<testsuites>\n"
         "  <testsuite name=\"%s\"\n"
         "             errors=\"%d\"\n"
         "             tests=\"%lu\"\n"
         "             failures=\"%lu\"\n"
         "             skipped=\"%lu\"\n"
         "             timestamp=\"%s\">\n",
         file_name,
         0,
         total,
         __tarsio_stats.total.fail,
         __tarsio_stats.total.skip,
         timestamp);

  for (node = __tarsio_result_list.first; NULL != node; node = node->next) {
    printf("    <testcase classname=\"%s\" name=\"%s\">\n", file_name, node->checkcase_name);
    switch(node->verdict) {
    case SKIP:
      puts("      <skipped/>");
      break;
    case FAIL:
      puts("      <failure message=\"test failure\">");      for (fnode = __tarsio_failure_list.first; NULL != fnode; fnode = fnode->next) {
        if (0 == strcmp(fnode->checkcase_name, node->checkcase_name)) {
          fprintf(stderr, "  %s:%lu:\n", fnode->file, (unsigned long int)fnode->line);
          fprintf(stderr, "    %s\n", fnode->help);
        }
      }
      puts("      </failure>");
      break;
    default:
      break;
    }
    puts("    </testcase>");
  }

  puts("  </testsuite>\n"
       "</testsuites>");
  return 0;
}

int __tarsio_summary(void) {
  int retval = 0;
  __tarsio_failure_node_t* fnode;
  char* last_check_name = NULL;
  if ((0 == __tarsio_options.compact) ||
      ((1 == __tarsio_options.compact) && (NULL != __tarsio_failure_list.first))) {
    putc('\n', stdout);
  }

  for (fnode = __tarsio_failure_list.first; NULL != fnode; fnode = fnode->next) {
    if (((NULL != last_check_name) && (0 != strcmp(last_check_name, fnode->checkcase_name))) || (NULL == last_check_name)) {
      fprintf(stderr, "%s:\n", fnode->checkcase_name);
    }
    fprintf(stderr, "  %s:%lu:\n", fnode->file, (unsigned long int)fnode->line);
    fprintf(stderr, "    %s\n", fnode->help);
    last_check_name = fnode->checkcase_name;
  }

  if (1 == __tarsio_options.verbose) {
    fprintf(stdout, "%lu success ", (unsigned long int)__tarsio_stats.total.pass);
    fprintf(stdout, "%lu failures ", (unsigned long int)__tarsio_stats.total.fail);
    fprintf(stdout, "%lu skipped ", (unsigned long int)__tarsio_stats.total.skip);
    fprintf(stdout, "\n");  }
  if (NULL != __tarsio_failure_list.first) {
    retval = -1;
  }

  return retval;
}

void __tarsio_cleanup(void) {
  __tarsio_failure_node_t* fnode = __tarsio_failure_list.first;
  __tarsio_result_node_t* rnode = __tarsio_result_list.first;
  while (NULL != rnode) {
    __tarsio_result_node_t* next_node = rnode->next;
    free(rnode);
    rnode = next_node;
  }
  while (NULL != fnode) {
    __tarsio_failure_node_t* next_node = fnode->next;
    free(fnode);
    fnode = next_node;
  }
}
