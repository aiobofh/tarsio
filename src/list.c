#include <stdlib.h>

#include "list.h"

int base_list_append(list_t* list, node_t* node) {
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
