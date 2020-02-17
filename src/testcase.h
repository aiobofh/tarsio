/*
 * Helpers for handling test-cases and test-suites
 *
 *              _______          _____ ___        ______
 *                 |      ||    |         |    | |      |
 *                 |      ||    |         |    | |      |
 *                 |   ___||___ |         |___ | |______|
 *
 *                   Copyleft AiO Secure Teletronics
 *
 * This is a collection of functions to parse test-case (check case) files
 * a.k.a. suites to automatically generate check-runner progarms with the
 * testcases executed and evaluated in the top-down order of how their
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

#ifndef _TESTCASE_H_
#define _TESTCASE_H_

#include "file.h"

enum testcase_type_e {
  TESTCASE_IS_UNKNOWN,
  TESTCASE_IS_UNIT_TEST,
  TESTCASE_IS_MODULE_TEST
};
typedef enum testcase_type_e testcase_type_t;

struct testcase_node_s {
  char* name;
  testcase_type_t type;
  struct testcase_node_s* next;
};
typedef struct testcase_node_s testcase_node_t;

#define TESTCASE_NODE_EMPTY (testcase_node_t){NULL, TESTCASE_IS_UNKNOWN, NULL}

struct testcase_list_s {
  testcase_node_t* first;
  testcase_node_t* last;
};
typedef struct testcase_list_s testcase_list_t;

#define TESTCASE_LIST_EMPTY (testcase_list_t){NULL, NULL}

int testcase_extract(testcase_list_t* list, const char* filename);

int testcase_list_init(testcase_list_t* list, const file_t* file);
void testcase_list_cleanup(testcase_list_t* list);

#endif
