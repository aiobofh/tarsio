/*
 * Helper functions for handling symbols and their usage
 *
 *              _______          _____ ___        ______
 *                 |      ||    |         |    | |      |
 *                 |      ||    |         |    | |      |
 *                 |   ___||___ |         |___ | |______|
 *
 *              Copyright (C) 2020 AiO Secure Teletronics
 *
 * These are helper functions to handle, sort and organize argument lists
 * for symols that may be used around the design under test. The lists are
 * mainly used for code replacement on a source level, where e.g. function
 * calls to specific functions are replaced with a call to a proxy-function
 * that enables the programmer to control the program flow better from the
 * testcases.
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
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "debug.h"
#include "symbol_usage.h"

static symbol_usage_node_t* symbol_usage_node_new(const size_t line, const size_t col, const size_t offset, const size_t last_function_start, const size_t last_function_line, void* prototype_node) {
  symbol_usage_node_t* node = malloc(sizeof(*node));
  if (NULL == node) {
    error0("Out of memory while allocating usage node");
    return NULL;
  }
  memset(node, 0, sizeof(*node));
  node->info.line = line;
  node->info.col = col;
  node->info.offset = offset;
  node->info.last_function_start = last_function_start;
  node->info.last_function_line = last_function_line;
  node->info.prototype_node = prototype_node; /* Loose connection via void* */
  return node;
}

void symbol_usage_append(symbol_usage_list_t* list, const size_t line, const size_t col, const size_t offset, const size_t last_function_start, const size_t last_function_line, void* prototype_node) {
  symbol_usage_node_t* node = symbol_usage_node_new(line, col, offset, last_function_start, last_function_line, prototype_node);
  if (NULL == node) {
    return;
  }
  debug3("  Symbol usage at line %lu col %lu offset %lu", line, col, offset);
  if (NULL == list->first) {
    list->first = node;
  }
  if (NULL != list->last) {
    list->last->next = node;
  }
  list->last = node;
  list->cnt++;
}

void symbol_usage_list_cleanup(symbol_usage_list_t* list) {
  symbol_usage_node_t* node;
  assert((NULL != list) && "Argument 'list' must not be NULL");
  node = list->first;
  while (NULL != node) {
    symbol_usage_node_t* next_node = node->next;
    free(node);
    node = next_node;
    list->cnt--;
  }
}
