#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "debug.h"
#include "symbol_usage.h"

static symbol_usage_node_t* symbol_usage_node_new(const size_t line, const size_t col, const size_t offset) {
  symbol_usage_node_t* node = malloc(sizeof(*node));
  if (NULL == node) {
    error0("Out of memory while allocating usage node");
    return NULL;
  }
  /*
  *node = (symbol_usage_node_t){NULL, {line, col, offset}};
  */
  memset(node, 0, sizeof(*node));
  node->info.line = line;
  node->info.col = col;
  node->info.offset = offset;

  return node;
}

void symbol_usage_append(symbol_usage_list_t* list, const size_t line, const size_t col, const size_t offset) {
  symbol_usage_node_t* node = symbol_usage_node_new(line, col, offset);
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

static void symbol_usage_node_cleanup(symbol_usage_node_t* node) {
#ifdef PARANOIA
  *node = (symbol_usage_node_t)SYMBOL_USAGE_NODE_EMPTY;
#else
  (void)node;
#endif
}

void symbol_usage_list_cleanup(symbol_usage_list_t* list) {
  symbol_usage_node_t* node = NULL;
  assert((NULL != list) && "Argument 'list' must not be NULL");
  node = list->first;
  while (NULL != node) {
    symbol_usage_node_t* next_node = node->next;
    symbol_usage_node_cleanup(node);
    free(node);
    node = next_node;
    list->cnt--;
  }
#ifdef PARANOIA
  *list = (symbol_usage_list_t)SYMBOL_USAGE_LIST_EMPTY;
#endif
}
