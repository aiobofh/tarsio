#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"

#include "argument.h"

int argument_list_append(argument_list_t* list, const char* data_type, const char* argument_name, const int is_const, const int is_variadic, const int astrisks) {
  argument_node_t* node = malloc(sizeof(*node));
  if (NULL == node) {
    error1("Out of memory while allocating argument node '%s'", data_type);
    return -1;
  }
  memset(node, 0, sizeof(*node));

  node->info.name = (char*)argument_name;
  node->info.datatype.name = (char*)data_type;
  node->info.datatype.datatype_definition.is_const = (int)is_const;
  node->info.datatype.datatype_definition.is_variadic = (int)is_variadic;
  node->info.datatype.datatype_definition.is_pointer = (int)astrisks;
  if (NULL == list->first) {
    list->first = node;
  }
  if (NULL != list->last) {
    list->last->next = node;
  }
  list->last = node;
  list->cnt++;
  return 0;
}

static void argument_cleanup(argument_t* argument) {
  datatype_cleanup(&argument->datatype);
  if (NULL != argument->name) {
    free(argument->name);
  }
  argument->name = NULL;
  argument->array_size = 0;
}

static void argument_node_cleanup(argument_node_t* node) {
  node->next = NULL;
  argument_cleanup(&node->info);
}

void argument_list_cleanup(argument_list_t* list) {
  argument_node_t* node;
  assert((NULL != list) && "Argument 'list' must not be NULL");
  node = list->first;
  while (NULL != node) {
    argument_node_t* next_node = node->next;
    argument_node_cleanup(node);
    free(node);
    node = next_node;
    list->cnt--;
  }
  memset(list, 0, sizeof(*list));
}
