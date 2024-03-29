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

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

#include "error.h"
#include "debug.h"

#include "argument.h"
#include "datatype.h"

argument_node_t* argument_node_new(char* data_type, char* argument_name,
                                   int is_const, int is_variadic,
                                   int astrisks) {
  argument_node_t* node = malloc(sizeof(*node));
  if (NULL == node) {
    error1("Out of memory while allocating argument node '%s'", data_type);
    return NULL;
  }
  memset(node, 0, sizeof(*node));

  node->info.name = (char*)argument_name;

  debug1("Argument: '%s'", node->info.name);

  datatype_init(&node->info.datatype, data_type, 0, 0,
                is_const, is_variadic, astrisks);

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
  if (NULL == list) {
    return;
  }
  node = list->first;
  while (NULL != node) {
    argument_node_t* next_node = node->next;
    argument_node_cleanup(node);
    free(node);
    node = next_node;
  }
}
