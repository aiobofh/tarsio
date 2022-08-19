/*
 * Helper functions for handling symbols and their usage
 *
 *              _______          _____ ___        ______
 *                 |      ||    |         |    | |      |
 *                 |      ||    |         |    | |      |
 *                 |   ___||___ |         |___ | |______|
 *
 *            Copyright (C) 2020-2022 AiO Secure Teletronics
 *
 * These are helper functions to handle, sort and organize argument lists
 * for symols that may be used around the design under test. The lists are
 * mainly used for code replacement on a source level, where e.g. function
 * calls to specific functions are replaced with a call to a proxy-function
 * that enables the programmer to control the program flow better from the
 * testcases.
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

#ifndef _SYMBOL_USAGE_H_
#define _SYMBOL_USAGE_H_

#include <stdlib.h>

#include "tokenizer.h"

struct symbol_usage_s {
  size_t line;
  size_t col;
  size_t offset;
  size_t last_function_start;
  size_t last_function_line;
  void* prototype_node;
};
typedef struct symbol_usage_s symbol_usage_t;

#define SYMBOL_USAGE_EMPTY {0, 0, 0, 0, 0, NULL}

struct symbol_usage_node_s {
  struct symbol_usage_node_s* next;
  symbol_usage_t info;
};
typedef struct symbol_usage_node_s symbol_usage_node_t;

#define SYMBOL_USAGE_NODE_EMPTY {NULL, SYMBOL_USAGE_EMPTY}

struct symbol_usage_list_s {
  size_t cnt;
  symbol_usage_node_t* first;
  symbol_usage_node_t* last;
};
typedef struct symbol_usage_list_s symbol_usage_list_t;

#define SYMBOL_USAGE_LIST_EMPTY {0, NULL, NULL}

void symbol_usage_append(symbol_usage_list_t* list, const size_t line, const size_t col, const size_t offset, const size_t last_function_start, const size_t last_function_line, void* prototype_node);

void symbol_usage_list_cleanup(symbol_usage_list_t* list);

symbol_usage_node_t*
symbol_usage_new_from_token(token_t* token, void* prototype_node);

void
symbol_usage_list_append_node(symbol_usage_list_t* list,
                              symbol_usage_node_t* node);

#endif
