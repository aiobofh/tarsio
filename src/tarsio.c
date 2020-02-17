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
 * runner execute and evaluate tests independently of each other.
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
#include <stdlib.h>
#include <string.h>

#include "tarsio.h"

struct __tarsio_stats_s {
  size_t success;
  size_t fail;
  size_t error;
  size_t skip;
};
typedef struct __tarsio_stats_s __tarsio_stats_t;

struct __tarsio_options_s {
  int verbose;
  int xml_output;
};
typedef struct __tarsio_options_s __tarsio_options_t;

struct __tarsio_failure_node_s {
  struct __tarsio_failure_node_s* next;
  char* testcase_name;
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
__tarsio_options_t __tarsio_options;
__tarsio_stats_t __tarsio_stats;

static void __tarsio_usage(const char* program_name) {
  printf("USAGE: %s <options>\n"
         " -h, --help    Show this help-text\n"
         " -v, --verbose Verbose output\n"
         " -x, --xml     Output JUnit compatible XML file\n"
         " -V, --version Show Tarsio version\n",
         program_name);
}

void __tarsio_append_failure(__tarsio_failure_list_t* list, const char* testcase_name, const char* help, const char* file, size_t line) {
  __tarsio_failure_node_t* node = malloc(sizeof(*node));

  if (NULL == node) {
    fprintf(stderr, "FATAL: Out of memory while allocating assert information in Tarsio\n");
    exit(EXIT_FAILURE);
  }

  node->testcase_name = (char*)testcase_name;
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

void __tarsio_assert_eq(int res, const char* testcase_name, const char* help, const char* file, size_t line) {
  if (res) {
    __tarsio_stats.fail++;
    __tarsio_append_failure(&__tarsio_failure_list, testcase_name, help, file, line);
  }
  else {
    __tarsio_stats.success++;
  }
}

void __tarsio_init(void) {
  memset(&__tarsio_stats, 0, sizeof(__tarsio_stats));
  memset(&__tarsio_failure_list, 0, sizeof(__tarsio_failure_list));
  memset(&__tarsio_options, 0, sizeof(__tarsio_options));
}

void __tarsio_handle_arguments(int argc, char* argv[]) {
  int i;
  for (i = 1; i < argc; i++) {
    if ((0 == strcmp(argv[i], "-v")) || (0 == strcmp(argv[i], "--verbose"))) {
      __tarsio_options.verbose = 1;
    }
    else if ((0 == strcmp(argv[i], "-x")) || (0 == strcmp(argv[i], "--xml"))) {
      __tarsio_options.xml_output = 1;
    }
    else if ((0 == strcmp(argv[i], "-h")) || (0 == strcmp(argv[i], "--help"))){
      __tarsio_usage(argv[0]);
      exit(EXIT_SUCCESS);
    }
  }
}

void __tarsio_skip(const char* reason, const char* test_name) {
  __tarsio_stats.skip++;
}

void __tarsio_unit_test_execute(__tarsio_data_t* __tarsio_mock_data, int (*func)(void* __tarsio_mock_data, const char* __tarsio_test_name), const char* name, size_t mock_data_size) {
  size_t skip = __tarsio_stats.skip;
  size_t fail = __tarsio_stats.fail;
  size_t error = __tarsio_stats.error;
  memset(__tarsio_mock_data, 0, mock_data_size);
  func(__tarsio_mock_data, name);
  if (1 == __tarsio_options.verbose) {
    if (skip != __tarsio_stats.skip) {
      printf("[SKIP] %s\n", name);
    }
    else if (error != __tarsio_stats.error) {
      printf("[ERROR] %s\n", name);
    }
    else if (fail != __tarsio_stats.fail) {
      printf("[FAIL] %s\n", name);
    }
    else {
      printf("[PASS] %s\n", name);
    }
  }
  else {
    if (skip != __tarsio_stats.skip) {
      putc('S', stdout);
    }
    else if (error != __tarsio_stats.error) {
      putc('E', stdout);
    }
    else if (fail != __tarsio_stats.fail) {
      putc('F', stdout);
    }
    else {
      putc('.', stdout);
    }
  }
}

int __tarsio_summary(void) {
  int retval = 0;
  __tarsio_failure_node_t* fnode;
  char* last_test_name = NULL;
  putc('\n', stdout);

  for (fnode = __tarsio_failure_list.first; NULL != fnode; fnode = fnode->next) {
    if (((NULL != last_test_name) && (0 != strcmp(last_test_name, fnode->testcase_name))) || (NULL == last_test_name)) {
      fprintf(stderr, "%s:\n", fnode->testcase_name);
    }
    fprintf(stderr, "  %s:%lu:\n", fnode->file, fnode->line);
    fprintf(stderr, "    %s\n", fnode->help);
    last_test_name = fnode->testcase_name;
  }

  if (NULL != __tarsio_failure_list.first) {
    retval = -1;
  }

  return retval;
}

void __tarsio_cleanup(void) {
  __tarsio_failure_node_t* fnode = __tarsio_failure_list.first;
  while (NULL != fnode) {
    __tarsio_failure_node_t* next_node = fnode->next;
    free(fnode);
    fnode = next_node;
  }
}
