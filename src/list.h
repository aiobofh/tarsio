/*
 * Generic linked list handling
 *
 *              _______          _____ ___        ______
 *                 |      ||    |         |    | |      |
 *                 |      ||    |         |    | |      |
 *                 |   ___||___ |         |___ | |______|
 *
 *              Copyright (C) 2020 AiO Secure Teletronics
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
