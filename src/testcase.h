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
