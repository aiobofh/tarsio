#include <stdio.h>
#include <stdlib.h>

#include "datatype.h"
#include "datatype_data.h"
#include "tarsio.h"

#define m tarsio_mock

test(datatype_init_shall_set_all_relevant_data) {
  datatype_t dt;
  unsigned char* ref = (unsigned char*)&dt;
  const unsigned char* endptr = ref + sizeof(dt) - 4;
  memset(&dt, 0, sizeof(dt));
  datatype_init(&dt, (char*)-1, -1, -1, -1, -1, -1);
  /* Check only the datatype definitions */
  ref += (sizeof(datatype_t) - sizeof(datatype_definition_t));
  while (endptr > ref) {
    assert_eq(0, (*ref == 0));
    ref++;
  }
}

test(datatype_cleanup_shall_free_datatype_name) {
  datatype_t dt;
  dt.name = (char*)1234;

  datatype_cleanup(&dt);

  assert_eq(1, m.free.call_count);
  assert_eq((char*)1234, m.free.args.arg0);
}

test(datatype_cleanup_shall_not_free_datatype_name_if_null) {
  datatype_t dt;
  dt.name = NULL;

  datatype_cleanup(&dt);

  assert_eq(0, m.free.call_count);
}
