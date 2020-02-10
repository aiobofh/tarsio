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
