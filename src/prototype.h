#ifndef _PROTOTYPE_H_
#define _PROTOTYPE_H_

#include "file.h"
#include "argument.h"
#include "symbol_usage.h"

struct raw_prototype_s {
  size_t decl_len;
  char* decl;
  char* args;
  size_t offset;
  size_t line;
  size_t column;
};
typedef struct raw_prototype_s raw_prototype_t;

#define RAW_PROTOTYPE_EMPTY {0, NULL, NULL, 0}

struct linkage_definition_s {
  int is_inline;
  int is_static;
  int is_extern;
  int is_declspec;
  int is_cdecl;
};
typedef struct linkage_definition_s linkage_definition_t;

#define LINKAGE_DEFINITION_EMPTY {0, 0, 0, 0}

struct prototype_s {
  raw_prototype_t raw_prototype;
  linkage_definition_t linkage_definition;
  datatype_t datatype;
  size_t symbol_len;
  char* symbol;
  argument_list_t argument_list;
  symbol_usage_list_t symbol_usage_list;
  size_t is_function_implementation;
};
typedef struct prototype_s prototype_t;

#define PROTOTYPE_EMPTY {RAW_PROTOTYPE_EMPTY, LINKAGE_DEFINITION_EMPTY, DATATYPE_EMPTY, 0, NULL, ARGUMENT_LIST_EMPTY, SYMBOL_USAGE_LIST_EMPTY}

struct prototype_node_s {
  struct prototype_node_s* next;
  prototype_t info;
};
typedef struct prototype_node_s prototype_node_t;

#define PROTOTYPE_NODE_EMPTY {NULL, PROTOTYPE_EMPTY}

struct prototype_list_s {
  char* filename;
  size_t size;
  size_t cnt;
  size_t first_function_implementation_offset;
  prototype_node_t* first;
  prototype_node_t* last;
};
typedef struct prototype_list_s prototype_list_t;

#define PROTOTYPE_LIST_EMPTY {NULL, 0, 0, 0, NULL, NULL}

int prototype_list_init(prototype_list_t* list, const file_t* file);
int prototype_usage(prototype_list_t* list, const file_t* file);
int prototype_remove_unused(prototype_list_t* list);
int prototype_extract_return_types(prototype_list_t* list);
int prototype_extract_arguments(prototype_list_t* list);
size_t prototype_get_first_function_implementation_line(prototype_list_t* list);
void prototype_list_cleanup(prototype_list_t* list);

void generate_prototype(prototype_node_t* node, const char* prefix, const char* prepend, const char* suffix);

#endif
