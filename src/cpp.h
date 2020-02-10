/*
 * C pre-processor list helpers
 *
 *              _______          _____ ___        ______
 *                 |      ||    |         |    | |      |
 *                 |      ||    |         |    | |      |
 *                 |   ___||___ |         |___ | |______|
 *
 *                   Copyleft AiO Secure Teletronics
 *
 * These are helper functions to handle, sort and organize the C
 * pre-processor directives in a parsed source-file. Keeping track of
 * #includes and putting them back in the correct order in generated code
 * and such things.
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

#define CPP_EMPTY {NULL}

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
