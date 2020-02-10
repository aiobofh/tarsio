/*
 * Helper functions for handling symbols and their usage
 *
 *              _______          _____ ___        ______
 *                 |      ||    |         |    | |      |
 *                 |      ||    |         |    | |      |
 *                 |   ___||___ |         |___ | |______|
 *
 *                   Copyleft AiO Secure Teletronics
 *
 * These are helper functions to handle, sort and organize argument lists
 * for symols that may be used around the design under test. The lists are
 * mainly used for code replacement on a source level, where e.g. function
 * calls to specific functions are replaced with a call to a proxy-function
 * that enables the programmer to control the program flow better from the
 * testcases.
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "debug.h"
#include "symbol_usage.h"

static symbol_usage_node_t* symbol_usage_node_new(const size_t line, const size_t col, const size_t offset, const size_t last_function_start, void* prototype_node) {
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
  node->info.prototype_node = prototype_node; /* Loose connection via void* */
  return node;
}

void symbol_usage_append(symbol_usage_list_t* list, const size_t line, const size_t col, const size_t offset, const size_t last_function_start, void* prototype_node) {
  symbol_usage_node_t* node = symbol_usage_node_new(line, col, offset, last_function_start, prototype_node);
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
