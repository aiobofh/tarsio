/*
 * C pre-processor list helpers
 *
 *              _______          _____ ___        ______
 *                 |      ||    |         |    | |      |
 *                 |      ||    |         |    | |      |
 *                 |   ___||___ |         |___ | |______|
 *
 *              Copyright (C) 2020 AiO Secure Teletronics
 *
 * These are helper functions to handle, sort and organize the C
 * pre-processor directives in a parsed source-file. Keeping track of
 * #includes and putting them back in the correct order in generated code
 * and such things.
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

#ifndef _CPP_H_
#define _CPP_H_

#include <stdlib.h>

#include "list.h"
#include "file.h"

struct cpp_s {
  char* directive;
};
typedef struct cpp_s cpp_t;

#define CPP_EMPTY {NULL, 0, 0}

struct cpp_node_s {
  NODE(struct cpp_node_s);
  cpp_t info;
};
typedef struct cpp_node_s cpp_node_t;

#define CPP_NODE_EMPTY {NULL, CPP_EMPTY}

struct cpp_list_s {
  LIST(cpp_node_t);
};
typedef struct cpp_list_s cpp_list_t;

#define CPP_LIST_EMPTY {0, NULL, NULL}

int cpp_list_init(cpp_list_t* list, const file_t* file);
void cpp_list_cleanup(cpp_list_t* list);

#endif
