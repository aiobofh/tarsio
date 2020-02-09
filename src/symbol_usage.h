#ifndef _SYMBOL_USAGE_H_
#define _SYMBOL_USAGE_H_

#include <stdlib.h>

struct symbol_usage_s {
  size_t line;
  size_t col;
  size_t offset;
  size_t last_function_start;
  void* prototype_node;
};
typedef struct symbol_usage_s symbol_usage_t;

#define SYMBOL_USAGE_EMPTY {0, 0, 0, 0, NULL}

struct symbol_usage_node_s {
  struct symbol_usage_node_s* next;
  symbol_usage_t info;
};
typedef struct symbol_usage_node_s symbol_usage_node_t;

#define SYMBOL_USAGE_NODE_EMPTY {NULL, SYMBOL_USAGE_EMPTY}

struct symbol_usage_list_s {
  size_t cnt;
  symbol_usage_node_t* first;
  symbol_usage_node_t* last;
};
typedef struct symbol_usage_list_s symbol_usage_list_t;

#define SYMBOL_USAGE_LIST_EMPTY {0, NULL, NULL}

void symbol_usage_append(symbol_usage_list_t* list, const size_t line, const size_t col, const size_t offset, const size_t last_function_start, void* prototype_node);

void symbol_usage_list_cleanup(symbol_usage_list_t* list);
#endif
