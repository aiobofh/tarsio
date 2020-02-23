/*
 * Helpers for handling check-cases and check-suites
 *
 *              _______          _____ ___        ______
 *                 |      ||    |         |    | |      |
 *                 |      ||    |         |    | |      |
 *                 |   ___||___ |         |___ | |______|
 *
 *              Copyright (C) 2020 AiO Secure Teletronics
 *
 * This is a collection of functions to parse test-case (check case) files
 * a.k.a. suites to automatically generate check-runner progarms with the
 * checkcases executed and evaluated in the top-down order of how their
 * specified in the cechk-case file.
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

#ifndef _CHECKCASE_H_
#define _CHECKCASE_H_

#include "file.h"

enum checkcase_type_e {
  CHECKCASE_IS_UNKNOWN,
  CHECKCASE_IS_UNIT_CHECK,
  CHECKCASE_IS_MODULE_CHECK
};
typedef enum checkcase_type_e checkcase_type_t;

struct checkcase_node_s {
  char* name;
  checkcase_type_t type;
  struct checkcase_node_s* next;
};
typedef struct checkcase_node_s checkcase_node_t;

#define CHECKCASE_NODE_EMPTY (checkcase_node_t){NULL, CHECKCASE_IS_UNKNOWN, NULL}

struct checkcase_list_s {
  checkcase_node_t* first;
  checkcase_node_t* last;
};
typedef struct checkcase_list_s checkcase_list_t;

#define CHECKCASE_LIST_EMPTY (checkcase_list_t){NULL, NULL}

int checkcase_extract(checkcase_list_t* list, const char* filename);

int checkcase_list_init(checkcase_list_t* list, const file_t* file);
void checkcase_list_cleanup(checkcase_list_t* list);

#endif
