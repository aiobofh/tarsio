#ifndef _ARGUMENT_H_
#define _ARGUMENT_H_

#include <stdlib.h>

#include "list.h"
#include "datatype.h"

struct argument_s {
  datatype_t datatype;
  char* name;
  size_t array_size;
};
typedef struct argument_s argument_t;

#define ARGUMENT_EMPTY {DATATYPE_EMPTY, NULL, 0}

struct argument_node_s {
  NODE(struct argument_node_s);
  argument_t info;
};
typedef struct argument_node_s argument_node_t;

#define ARGUMENT_NODE_EMPTY {NULL, ARGUMENT_EMPTY}

struct argument_list_s {
  LIST(argument_node_t);
};
typedef struct argument_list_s argument_list_t;

#define ARGUMENT_LIST_EMPTY LIST_EMPTY

argument_node_t* argument_node_new(char* data_type, char* argument_name, int is_const, int is_variadic, int astrisks);
int argument_list_append(argument_list_t* list, argument_node_t* node);
void argument_list_cleanup(argument_list_t* list);

#endif
