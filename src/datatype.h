#ifndef _DATATYPE_H_
#define _DATATYPE_H_

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
  char* name;
  datatype_definition_t datatype_definition;
};
typedef struct datatype_s datatype_t;

#define DATATYPE_EMPTY {NULL, DATATYPE_DEFINITION_EMPTY}

void datatype_init(datatype_t* datatype, char* name, int is_static, int is_function_pointer, int is_const, int is_variadic, int astrisks);
void datatype_cleanup(datatype_t* datatype);

#endif
