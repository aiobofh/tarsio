#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

#include "error.h"

#include "argument.h"
#include "datatype.h"

argument_node_t* argument_node_new(char* data_type, char* argument_name, int is_const, int is_variadic, int astrisks) {
  argument_node_t* node = malloc(sizeof(*node));
  if (NULL == node) {
    error1("Out of memory while allocating argument node '%s'", data_type);
    return NULL;
  }
  memset(node, 0, sizeof(*node));

  node->info.name = (char*)argument_name;

  datatype_init(&node->info.datatype, data_type, 0, 0, is_const, is_variadic, astrisks);

  return node;
}

int argument_list_append(argument_list_t* list, argument_node_t* node) {
  return list_append(list, node);
}

static void argument_cleanup(argument_t* argument) {
  datatype_cleanup(&argument->datatype);
  if (NULL != argument->name) {
    free(argument->name);
  }
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
  }
}
