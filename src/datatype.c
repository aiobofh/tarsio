#include <stdlib.h>
#include <string.h>

#include "datatype.h"

void datatype_init(datatype_t* datatype, char* name, int is_static, int is_function_pointer, int is_const, int is_variadic, int astrisks) {
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
