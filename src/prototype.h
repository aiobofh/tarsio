/*
 * Function prototype parsing and helper functions
 *
 *              _______          _____ ___        ______
 *                 |      ||    |         |    | |      |
 *                 |      ||    |         |    | |      |
 *                 |   ___||___ |         |___ | |______|
 *
 *            Copyright (C) 2020-2022 AiO Secure Teletronics
 *
 * Function headers are parsed and interpreted to be transformed to other
 * formats and test-suite data storage datatypes. But also to generate
 * extern declarations in the correct place of the source code to ensure
 * that the program compiles with close to zero incrase of work load for
 * the programmer.
 *
 * Beware, this code is quite complex and also the very heart of Tarsio,
 * so I can not say it enough times: Here be dragons!
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

#ifndef _PROTOTYPE_H_
#define _PROTOTYPE_H_

#include "file.h"
#include "argument.h"
#include "symbol_usage.h"
#include "tokenizer.h"

struct raw_prototype_s {
  size_t decl_len;
  char* decl;
  char* args;
  size_t offset;
  size_t line;
  size_t column;
};
typedef struct raw_prototype_s raw_prototype_t;

#define RAW_PROTOTYPE_EMPTY {0, NULL, NULL, 0, 0, 0, 0, NULL}

struct linkage_definition_s {
  int is_inline;
  int is_static;
  int is_extern;
  int is_declspec;
  int is_cdecl;
};
typedef struct linkage_definition_s linkage_definition_t;

#define LINKAGE_DEFINITION_EMPTY {0, 0, 0, 0}

struct prototype_s {
  raw_prototype_t raw_prototype;
  linkage_definition_t linkage_definition;
  token_node_t* token_node;
  datatype_t datatype;
  size_t symbol_len;
  char* symbol;
  argument_list_t argument_list;
  symbol_usage_list_t symbol_usage_list;
  size_t is_function_implementation;
  size_t is_function_implementation_line;
};
typedef struct prototype_s prototype_t;

#define PROTOTYPE_EMPTY {                       \
      RAW_PROTOTYPE_EMPTY,                      \
      LINKAGE_DEFINITION_EMPTY,                 \
      DATATYPE_EMPTY,                           \
      0,                                        \
      NULL,                                     \
      ARGUMENT_LIST_EMPTY,                      \
      SYMBOL_USAGE_LIST_EMPTY}

struct prototype_node_s {
  struct prototype_node_s* next;
  prototype_t info;
};
typedef struct prototype_node_s prototype_node_t;

#define PROTOTYPE_NODE_EMPTY {NULL, PROTOTYPE_EMPTY}

struct prototype_list_s {
  const char* filename;
  size_t size;
  size_t cnt;
  size_t first_function_implementation_offset;
  prototype_node_t* first;
  prototype_node_t* last;
};
typedef struct prototype_list_s prototype_list_t;

#define PROTOTYPE_LIST_EMPTY {NULL, 0, 0, 0, NULL, NULL}

int prototype_list_init(prototype_list_t* list, const file_t* file);
int prototype_list_init_from_tokens(prototype_list_t* list, token_list_t* token_list);
int prototype_usage(prototype_list_t* list, const file_t* file);
int prototype_usage_from_tokens(prototype_list_t* list, token_list_t* token_list);
int prototype_remove_unused(prototype_list_t* list);
int prototype_extract_return_types(prototype_list_t* list);
int prototype_extract_arguments(prototype_list_t* list);
size_t prototype_get_first_function_implementation_line(prototype_list_t* list);
void prototype_list_cleanup(prototype_list_t* list);

void generate_prototype(prototype_node_t* node, const char* prefix,
                        const char* prepend, const char* suffix);

#endif
