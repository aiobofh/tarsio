#ifndef _LIST_H_
#define _LIST_H_

#include <stdlib.h>

#define NODE(NODETYPE)                          \
  NODETYPE* next

#define LIST(NODETYPE)                          \
  size_t cnt;                                   \
  NODETYPE* first;                              \
  NODETYPE* last

#define list_append(LISTPTR, NODEPTR) base_list_append((list_t*)LISTPTR, (node_t*)NODEPTR)

struct node_s {
  NODE(struct node_s);
};

typedef struct node_s node_t;

#define NODE_EMPTY {NULL}

struct list_s {
  LIST(node_t);
};

#define LIST_EMPTY {0, NULL, NULL}

typedef struct list_s list_t;

int base_list_append(list_t* list, node_t* node);

#endif
