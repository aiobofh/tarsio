/*
 * Function argument/parameter list helpers
 *
 *              _______          _____ ___        ______
 *                 |      ||    |         |    | |      |
 *                 |      ||    |         |    | |      |
 *                 |   ___||___ |         |___ | |______|
 *
 *            Copyright (C) 2020-2022 AiO Secure Teletronics
 *
 * These are helper functions to handle, sort and organize argument lists
 * for functions. These are used to generate mockup-functions and
 * function prototypes.
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
