#include <stdlib.h>
#include <stdio.h>

#include "testcase.h"
#include "testcase_data.h"
#include "tarsio.h"

#define m tarsio_mock

test(testcase_list_init_shall_call_parse_correctly) {
  testcase_list_init((testcase_list_t*)0x1234, (file_t*)0x5678);

  assert_eq(1, m.parse.call_count);
  assert_eq((testcase_list_t*)0x1234, m.parse.args.arg0);
  assert_eq((file_t*)0x5678, m.parse.args.arg1);
}

test(parse_shall_mallocate_the_correct_number_of_bytes_plus_one_for_null) {
  file_t file;
  file.len = 5678;
  parse((testcase_list_t*)0x1234, &file);

  assert_eq(1, m.malloc.call_count);
  assert_eq(5678 + 1, m.malloc.args.arg0);
}
