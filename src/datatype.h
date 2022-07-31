/*
 * Datatype helpers, for return types, srtucts and function arguments
 *
 *              _______          _____ ___        ______
 *                 |      ||    |         |    | |      |
 *                 |      ||    |         |    | |      |
 *                 |   ___||___ |         |___ | |______|
 *
 *            Copyright (C) 2020-2022 AiO Secure Teletronics
 *
 * These are helper functions to handle, sort and organize the datatypes
 * used in the design under test. These are parsed and interpreted to
 * be ble to produce mockup-functions, storage data types for tests and
 * function prototype generators.
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

#ifndef _DATATYPE_H_
#define _DATATYPE_H_

#include <stdlib.h>

struct datatype_definition_s {
  int is_static;
  int is_pointer;
  int is_const;
  int is_function_pointer;
  int is_variadic;
};
typedef struct datatype_definition_s datatype_definition_t;

#define DATATYPE_DEFINITION_EMPTY {0, 0, 0, 0, 0}

struct datatype_s {
  size_t name_len;
  char* name;
  datatype_definition_t datatype_definition;
};
typedef struct datatype_s datatype_t;

#define DATATYPE_EMPTY {0, NULL, DATATYPE_DEFINITION_EMPTY}

void datatype_init(datatype_t* datatype, char* name, int is_static,
                   int is_function_pointer, int is_const, int is_variadic,
                   int astrisks);
void datatype_cleanup(datatype_t* datatype);

#endif
