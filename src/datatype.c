#include <stdlib.h>
#include <string.h>

#include "datatype.h"

void datatype_cleanup(datatype_t* datatype) {
  if (NULL != datatype->name) {
    free(datatype->name);
  }
  /*
  *datatype = (datatype_t)DATATYPE_EMPTY;
  */
  memset(datatype, 0, sizeof(*datatype));
}
