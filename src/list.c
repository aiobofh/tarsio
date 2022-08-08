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
#include "list.h"
#include "helpers.h"

#include <stdlib.h>


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

void _list_add(__list_t* list, __node_t* node) {
  assert(NULL != list);
  assert(NULL != node);
  assert(NULL == node->__prev && "Only detached nodes can be added");
  assert(NULL == node->__next && "Only detached nodes can be added");

  node->__next = NULL;
  node->__prev = list->__last;
  if (NULL != list->__last) {
    list->__last->__next = node;
  }
  list->__last = node;

  if (NULL == list->__first) {
    list->__first = node;
  }

  list->__cnt++;
}

void _list_insert(__list_t* list, __node_t* before, __node_t* node) {
  assert(NULL != list);
  assert(NULL != before);
  assert(NULL != node);
  assert(NULL == node->__prev && "Only detached nodes can be inserted");
  assert(NULL == node->__next && "Only detached nodes can be inserted");
  assert(NULL != list->__first && "Can not insert into empty list");
  assert(NULL != list->__last && "Can not insert into empty list");

  node->__next = before;
  node->__prev = before->__prev;
  if (before->__prev) {
    before->__prev->__next = node;
  }
  before->__prev = node;
  if (list->__first == before) {
    list->__first = node;
  }

  list->__cnt++;
}


void _list_remove(__list_t* list, __node_t* node) {
  assert(NULL != list);
  assert(NULL != node);

  if (list->__last == node) {
    list->__last = node->__prev;
  }
  if (list->__first == node) {
    list->__first = node->__next;
  }
  if (node->__prev) {
    node->__prev->__next = node->__next;
  }
  if (node->__next) {
    node->__next->__prev = node->__prev;
  }

  node->__prev = NULL;
  node->__next = NULL;

  list->__cnt--;
}

size_t _list_count(__list_t* list) {
  assert(NULL != list);
  return list->__cnt;
}

__node_t** _list_compile_array(__list_t* list, __node_t*** array, size_t size){
  enum {
    ok = 0,
    malloc_failed = -1
  } retval = ok;
  __node_t* node;
  size_t i = 0;
  size_t cnt;
  assert(NULL != list);

  cnt = list->__cnt;

  if (0 == cnt) {
    return NULL;
  }

  *array = smalloc(size * (cnt + 1)) else ret(malloc_failed);

  for (node = first(list); node; node = next(node)) {
    (*array)[i++] = node;
  }
  (*array)[i++] = NULL;

 malloc_failed:

  return (ok == retval) ? (*array) : NULL;
}
