/*
 * Generic linked list handling
 *
 *              _______          _____ ___        ______
 *                 |      ||    |         |    | |      |
 *                 |      ||    |         |    | |      |
 *                 |   ___||___ |         |___ | |______|
 *
 *            Copyright (C) 2020-2022 AiO Secure Teletronics
 *
 * Just because linked lists are so fun, here are som helpers :)
 *
 *  This file is part of Tarsio.
 *
 *  Tarsio is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Tarsio is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Tarsio.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#ifndef _LIST_H_
#define _LIST_H_

/* TODO: This is the old stuff, remove it when done */
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


/*****************************************************************************
 * This is the new stuff
 */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "node.h"

enum __list_type_e
  {
   LIST_TYPE_LIST = 0,
  };
typedef enum __list_type_e __list_type_t;

/* Use this macro to "inherit" the default node data types */
#define LIST_STRUCT(t)                          \
  __list_type_t __type;                         \
  t* __first;                                   \
  t* __last;                                    \
  size_t __cnt

struct __list_s {
  LIST_STRUCT(__node_t);
};
typedef struct __list_s __list_t;

/* Use this to initialize an empty list, if you want to */
#define LIST_INIT LIST_TYPE_LIST, NULL, NULL, 0

/*
 * Macros to make the call to list manipulation functions type agnostic,
 * and not produce compiler warnigns.
 */
#define list_add(l,n)                           \
  do {                                          \
    assert(LIST_TYPE_LIST == (l)->__type);      \
    assert(NODE_TYPE_NODE == (n)->__type);      \
    _list_add((__list_t*)(l), (__node_t*)(n));  \
  } while (0)

#define list_insert(l,b,n)                                              \
  do {                                                                  \
    assert(LIST_TYPE_LIST == (l)->__type);                              \
    assert(NODE_TYPE_NODE == (b)->__type);                              \
    assert(NODE_TYPE_NODE == (n)->__type);                              \
    _list_insert((__list_t*)(l), (__node_t*)(b), (__node_t*)(n));       \
  } while (0)

#define list_remove(l,n)                                \
  do {                                                  \
    assert(LIST_TYPE_LIST == (l)->__type);              \
    assert(NODE_TYPE_NODE == (n)->__type);              \
    _list_remove((__list_t*)(l), (__node_t*)(n));       \
  } while (0)

#define list_count(l)                           \
  _list_count((__list_t*)(l))

#define list_swap(l,a,b)                                                \
  do {                                                                  \
    assert(LIST_TYPE_LIST == (l)->__type);                              \
    assert(NODE_TYPE_NODE == (a)->__type);                              \
    assert(NODE_TYPE_NODE == (b)->__type);                              \
    _list_swap((__list_t*)l, (__node_t*)a, (__node_t*)b);               \
  } while (0)

#define list_compile_array(l, a)                                        \
  do {                                                                  \
    assert(LIST_TYPE_LIST == (l)->__type);                              \
    _list_compile_array((__list_t*)l, (__node_t***)a, sizeof(*a), malloc); \
  } while (0)

/*
 * Helper functions with short names to make for-loops using lists become
 * nicer for the eyes.
 *
 * Example:
 *
 * for (n = first(l); n; n = next(n)) {
 *   // Your code
 * }
 *
 */
/*
#define first(l) (void*)((l)->__first)
#define last(l) (void*)((l)->__last)
#define prev(n) (void*)((n)->__prev)
#define next(n) (void*)((n)->__next)
*/
#define first(l) ((l)->__first)
#define last(l) ((l)->__last)
#define prev(n) ((n)->__prev)
#define next(n) ((n)->__next)

#define count(l) ((l)->__cnt)
/*
#define each(l, n) (n) = (void*)first((l)); (n); (n) = (void*)next((n))
*/
#define each(l, n) (n) = first((l)); (n); (n) = next((n))
#define is_empty(l) (NULL == first(l))

/* Actual implementations without wrapper macros */
void _list_add(__list_t* list, __node_t* node);
void _list_insert(__list_t* list, __node_t* before, __node_t* node);
void _list_remove(__list_t* list, __node_t* node);
size_t _list_count(__list_t* list);
void _list_swap(__list_t* list, __node_t *a, __node_t *b);

/* If you want to save about 10% CPU time an indexed array is faster,
 * only use this if you do MANY sequential reads once the list is
 * arranged the way you want it to be. */
__node_t** _list_compile_array(__list_t* list, __node_t*** array, size_t size);


#endif
