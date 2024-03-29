/*
 * C pre-processor list helpers
 *
 *              _______          _____ ___        ______
 *                 |      ||    |         |    | |      |
 *                 |      ||    |         |    | |      |
 *                 |   ___||___ |         |___ | |______|
 *
 *            Copyright (C) 2020-2022 AiO Secure Teletronics
 *
 * These are helper functions to handle, sort and organize the C
 * pre-processor directives in a parsed source-file. Keeping track of
 * #includes and putting them back in the correct order in generated code
 * and such things.
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

#include <assert.h>
#include <string.h>

#include "error.h"
#include "debug.h"
#include "list.h"

#include "str.h"
#include "file.h"
#include "cpp.h"

static cpp_node_t* cpp_node_new(const char* buf) {
  cpp_node_t* node;

  if (NULL == (node = malloc(sizeof(*node)))) {
    error1("Out of memory while allocating cpp directive for '%s'", buf);
    goto node_malloc_failed;
  }

  memset(node, 0, sizeof(*node));

  if (NULL == (node->info.directive = strclone(buf))) {
    goto strclone_failed;
  }

  goto normal_exit;

 strclone_failed:
  free(node);
  node = NULL;
 node_malloc_failed:
 normal_exit:
  return node;
}

static void cpp_list_append(cpp_list_t* list, const char* buf) {
  cpp_node_t* node = cpp_node_new(buf);

  if (NULL == node) {
    return;
  }

  list_append(list, node);
}

static int extract_cpp_directives(void* list_ptr, file_parse_state_t* state, const char c, const size_t line, const size_t col, const size_t offset, const size_t last_function_start, const size_t last_function_line) {
  /*
   * This is a bit of guess-work. If the code under test and the test-cases
   * are written with some kind of dicipline we'll be fine with this
   * approach. And since every programmer take pride in claiming they're
   * the best i think there will be no real problems here :D
   */
  cpp_list_t* list = (cpp_list_t*)list_ptr;
#ifndef VBCC /* sigh... */
  (void)line;
  (void)col;
  (void)offset;
  (void)last_function_start;
  (void)last_function_line;
#endif
  if (('#' == c) && (('\n' == state->last_c) || (' ' == state->last_c))) {
    state->idx = 0;
    state->buf[state->idx] = '\0';
  }

  if (('\\' != state->last_c) && (('\n' == c) || ('\r' == c))) {
    if ('#' == state->buf[0]) {
      /* BOFH!!! Verify that buf == should be here */
      if (state->buf == strstr(state->buf, "#ifndef _TARSIO_DATA_")) {
        /* Prevent multiple declarations of the generated things */
	debug1("%s", state->buf);
	cpp_list_append(list, "#ifdef _TARSIO_DATA_");
      }
      else {
	debug1("Found CPP direciive '%s'", state->buf);
	cpp_list_append(list, state->buf);
        debug0("Added to list");
      }
    }
    state->buf[0] = '\0';
    state->idx = 0;
  }
  else {
    state->buf[state->idx++] = c;
    state->buf[state->idx] = '\0';
  }

  /*
   * Try to only take the top part of the file. The includes are the most
   * important to catch.
   */
  if ((state->buf == strstr(state->buf, "test(")) ||
      (state->buf == strstr(state->buf, "module_test("))) {
    state->done = 1;
  }

  state->last_c = c;

  return 0;
}


int cpp_list_init(cpp_list_t* list, const file_t* file) {
  assert((NULL != list) && "Argument 'list' must not be NULL");
  assert((NULL != file) && "Argument 'file' must not be NULL");

  if ((NULL == list) || (NULL == file)) {
    /* Even if asserts are not compiled in, return */
    return -1;
  }

  file_parse(extract_cpp_directives, list, file, PARSE_DECLARATIONS, 0);

  return 0;
}

static void cpp_node_cleanup(cpp_node_t* node) {
  if (node->info.directive) {
    free(node->info.directive);
  }
  free(node);
}

void cpp_list_cleanup(cpp_list_t* list) {
  cpp_node_t* node;

  assert((NULL != list) && "Argument 'list' must not be NULL");

  if (NULL == list) { /* Even if asserts are not compiled in, return */
    return;
  }

  node = list->first;
  while (NULL != node) {
    cpp_node_t* next_node = node->next;
    cpp_node_cleanup(node);
    node = next_node;
  }
}
