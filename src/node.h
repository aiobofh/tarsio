#ifndef _NODE_H_
#define _NODE_H_

#include <stdlib.h>

enum __node_type_e
  {
   NODE_TYPE_NODE = 0,
  };
typedef enum __node_type_e __node_type_t;

/* Use this macro to "inherit" the default node data types */
#define NODE_STRUCT(TYPE)                       \
  __node_type_t __type;                         \
  TYPE* __prev;                                 \
  TYPE* __next

struct __node_s {
  NODE_STRUCT(struct __node_s);
};
typedef struct __node_s __node_t;

#define NODE_INIT NODE_TYPE_NODE, NULL, NULL

#define node_malloc(size) __safepcall(_node_malloc(size))
__node_t* _node_malloc(size_t size);

#endif
