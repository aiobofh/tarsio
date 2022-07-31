/*
 * Helpers for handling check-cases and check-suites
 *
 *              _______          _____ ___        ______
 *                 |      ||    |         |    | |      |
 *                 |      ||    |         |    | |      |
 *                 |   ___||___ |         |___ | |______|
 *
 *            Copyright (C) 2020-2022 AiO Secure Teletronics
 *
 * This is a collection of functions to parse test-case (check case) files
 * a.k.a. suites to automatically generate check-runner progarms with the
 * check-cases executed and evaluated in the top-down order of how their
 * specified in the cechk-case file.
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

#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "debug.h"
#include "error.h"

#include "str.h"

#include "file.h"
#include "checkcase.h"

#ifndef MAX_FUNC_NAME_LEN
#define MAX_FUNC_NAME_LEN 78 /* Some compilers are limited to 32 char */
#endif

static void checkcase_list_append(checkcase_list_t* list, checkcase_node_t* node) {
  if (NULL == list->first) {
    list->first = node;
  }
  if (NULL != list->last) {
    list->last->next = node;
  }
  list->last = node;
}

static checkcase_node_t* new_node(char* name, checkcase_type_t type) {
  checkcase_node_t* node;

  if (NULL == (node = malloc(sizeof(*node)))) {
    error1("Out of memory while allocating check-case node for '%s'", name);
    goto checkcase_node_malloc_failed;
  }

  memset(node, 0, sizeof(*node));

  node->type = type;
  node->name = name;

 checkcase_node_malloc_failed:
  return node;
}

static void print_len_warning(const char* str) {
  fprintf(stderr, "WARNING: '%s' is a long check-case name, some compilers may trunkate it - resulting in duplicate symbol names.\n", str);
}


static int checkcase_append(checkcase_list_t* list, const char* name, const checkcase_type_t type) {
  int retval = 0;
  char* node_name;
  checkcase_node_t* node;

  const size_t len = strlen(name);

  if (len >= MAX_FUNC_NAME_LEN) {
    print_len_warning(name);
  }

  if (NULL == (node_name = strclone(name))) {
    error1("Unable to clone check-case name '%s'", name);
    retval = -1;
    goto strclone_failed;
  }

  if (NULL == (node = new_node(node_name, type))) {
    error1("Unable to create new check-case node '%s'", name);
    retval = -2;
    goto new_node_failed;
  }

  checkcase_list_append(list, node);

  goto normal_exit;

 new_node_failed:
 strclone_failed:
 normal_exit:
  return retval;
}

static char *index_of(char* buf, char c) {
  while (*buf != c) {
    buf++;
  }
  return (char*)buf;
}

static int extract(checkcase_list_t* list, char* buf) {
  int retval = 0;
  char* name = NULL;
  char* ptr;
  checkcase_type_t type = CHECKCASE_IS_UNKNOWN;

  if (buf == strstr(buf, "test(")) {
    name = buf + strlen("test(");
    type = CHECKCASE_IS_UNIT_CHECK;
  }
  else if (buf == strstr(buf, "check(")) {
    name = buf + strlen("check(");
    type = CHECKCASE_IS_UNIT_CHECK;
  }
  else if (buf == strstr(buf, "module_test(")) {
    name = buf + strlen("module_test(");
    type = CHECKCASE_IS_MODULE_CHECK;
  }
  else if (buf == strstr(buf, "module_check(")) {
    name = buf + strlen("module_check(");
    type = CHECKCASE_IS_MODULE_CHECK;
  }

  if (NULL == name) {
    goto normal_exit;
  }


  if (NULL == (ptr = index_of(name, ')'))) {
    error1("Could not find check-case name length in '%s'", name);
    retval = -1;
    goto index_failed;
  }

  *ptr = '\0';

  if (0 != checkcase_append(list, name, type)) {
    error1("Unable to append '%s' to check-case list", name);
    retval = -2;
    goto checkcase_append_failed;
  }

  goto normal_exit;

 checkcase_append_failed:
 index_failed:
 normal_exit:
  return retval;
}

int call = 0;

static int parse(checkcase_list_t* list, const file_t* file) {
  const size_t len = file->len;

  int retval = 0;
  char* buf;
  char* p = file->buf;
  char last_c = 0;
  size_t i = 0;
  size_t idx = 0;

  int is_in_block_comment = 0;
  int is_in_row_comment = 0;
  int is_in_comment = 0;
  int is_in_body = 0;

  /*
   * This is overdoing it a bit, since we probably only need a few bytes,
   * but we never know if the buffer will be full och crap.
   */
  if (NULL == (buf = malloc(file->len + 1))) {
    error1("Out of memory while allocating buffert for check-cases in '%s'", file->filename);
    retval = -1;
    goto buf_malloc_failed;
  }

  buf[0] = '\0';

  while (i < len) {
    const char c = p[i];
    const int is_block_start = (!is_in_body && !is_in_comment && ('{' == c));
    const int is_block_end = (is_in_body && !is_in_comment && ('}' == c));
    const int is_block_comment_start = (!is_in_body && (('/' == last_c) && ('*' == c)));
    const int is_block_comment_end = (!is_in_body && (('*' == last_c) && ('/' == c)));
    const int is_row_comment_start = (!is_in_body && (('/' == last_c) && ('/' == c)));
    const int is_row_comment_end = (is_in_row_comment && !is_in_body && ('\n' == c));
    const int is_line_feed = ('\n' == c);
    const int is_end_of_declaration = (!is_in_body && !is_in_comment && (is_line_feed || ('{' == c)));

    is_in_body += is_block_start;
    is_in_block_comment += is_block_comment_start;
    is_in_row_comment += is_row_comment_start;

    is_in_comment = (is_in_block_comment || is_in_row_comment);

    if (is_end_of_declaration) {
      if (0 != extract(list, buf)) {
        retval = -2;
        goto extract_failed;
      }
      idx = 0;
    }

    if (is_line_feed) {
      idx = 0;
    }
    else {
      buf[idx++] = c;
    }

    buf[idx] = '\0';

    is_in_block_comment -= is_block_comment_end;
    is_in_row_comment -= is_row_comment_end;
    is_in_body -= is_block_end;

    last_c = c;
    i++;
  }


 extract_failed:
  free(buf);
 buf_malloc_failed:
  return retval;
}

int checkcase_list_init(checkcase_list_t* list, const file_t* file) {
  assert((NULL != list) && "Argument 'list' must not be NULL");
  assert((NULL != file) && "Argument 'file' must not be NULL");

  return parse(list, file);
}

static void checkcase_node_cleanup(checkcase_node_t* node) {
  if (NULL != node->name) {
    free(node->name);
  }
  free(node);
}

void checkcase_list_cleanup(checkcase_list_t* list) {
  checkcase_node_t* node = list->first;
  while (NULL != node) {
    checkcase_node_t* next_node = node->next;
    checkcase_node_cleanup(node);
    node = next_node;
  }
}
