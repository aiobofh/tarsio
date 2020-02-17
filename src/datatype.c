/*
 * Datatype helpers, for return types, srtucts and function arguments
 *
 *              _______          _____ ___        ______
 *                 |      ||    |         |    | |      |
 *                 |      ||    |         |    | |      |
 *                 |   ___||___ |         |___ | |______|
 *
 *                   Copyleft AiO Secure Teletronics
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

#include <stdlib.h>
#include <string.h>

#include "datatype.h"

void datatype_init(datatype_t* datatype, char* name, int is_static,
                   int is_function_pointer, int is_const,
                   int is_variadic, int astrisks) {
  datatype->name = name;
  datatype->datatype_definition.is_static = (int)is_static;
  datatype->datatype_definition.is_pointer = (int)astrisks;
  datatype->datatype_definition.is_const = (int)is_const;
  datatype->datatype_definition.is_function_pointer = (int)is_function_pointer;
  datatype->datatype_definition.is_variadic = (int)is_variadic;
}

void datatype_cleanup(datatype_t* datatype) {
  if (NULL != datatype->name) {
    free(datatype->name);
  }
  memset(datatype, 0, sizeof(*datatype));
}
