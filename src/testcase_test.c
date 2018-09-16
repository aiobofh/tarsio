#include <stdio.h>

#include "testcase.h"
#include "/tmp/testcase_tarsio_data.h"
#include "tarsio.h"

#define m tarsio_mock

test(testcase_list_init_shall_call_parse_correctly) {
  (void)__tarsio_mock_data;

  testcase_list_init((testcase_list_t*)0x1234, (file_t*)0x5678);
  printf("Tjosan\n");

  assert_eq(1, m.parse.call_count);
  assert_eq((testcase_list_t*)0x1234, m.parse.args.arg0);
  assert_eq((file_t*)0x5678, m.parse.args.arg1);
}
