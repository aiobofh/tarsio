#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "testcase.h"
#include "testcase_data.h"
#include "tarsio.h"

#define m tarsio_mock

/***************************************************************************
 * index_of()
 */
test(index_of_shall_return_the_pointer_to_the_position_of_a_specific_char) {
  (void)m;
  char* teststring = "12345";
  assert_eq(&teststring[4], index_of(teststring, '5'));
}

/***************************************************************************
 * extract()
 */
test(extract_return_0_on_success) {
  (void)m;
  assert_eq(0, extract(NULL, NULL));
}

test(extract_search_for_unit_test_names) {
  m.strstr.retval = (char*)0x1234;

  extract(NULL, (char*)0x1234);

  assert_eq(1, m.strstr.call_count);
  assert_eq(0, strcmp("test(", m.strstr.args.arg1));
}

test(extract_search_for_module_test_names) {
  m.strstr.retval = NULL;

  extract(NULL, (char*)0x1234);

  assert_eq(2, m.strstr.call_count);
  assert_eq(0, strcmp("module_test(", m.strstr.args.arg1));
}

test(extract_search_for_en_parentheis_using_index_of) {
  m.strstr.func = strstr;
  m.strlen.func = strlen;

  extract(NULL, "test(a_name)");

  assert_eq(1, m.index_of.call_count);
  assert_eq(0, strcmp("a_name)", m.index_of.args.arg0));
  assert_eq(')', m.index_of.args.arg1);
}

test(extract_return_negative_1_if_parentheis_was_not_found) {
  m.strstr.func = strstr;
  m.strlen.func = strlen;

  m.index_of.retval = NULL;

  assert_eq(-1, extract(NULL, "test(a_name)"));
}

test(extract_not_append_test_case_if_name_not_found) {
  m.strstr.func = strstr;
  m.strlen.func = strlen;

  m.index_of.retval = NULL;

  extract(NULL, "test(a_name)");

  assert_eq(0, m.testcase_append.call_count);
}

test(extract_append_test_case_if_name_not_found) {
  char teststring[14];
  strcpy(teststring, "test(a_name)");
  m.strstr.func = strstr;
  m.strlen.func = strlen;

  m.index_of.retval = teststring + strlen("test(");

  extract(NULL, teststring);

  assert_eq(1, m.testcase_append.call_count);
}

test(extract_call_append_test_correctly) {
  char teststring[14];
  strcpy(teststring, "test(a_name)");
  m.strstr.func = strstr;
  m.strlen.func = strlen;

  m.index_of.retval = teststring + strlen(teststring) - 1;

  extract((testcase_list_t*)0x1234, teststring);

  assert_eq(1, m.testcase_append.call_count);
  assert_eq((testcase_list_t*)0x1234, m.testcase_append.args.arg0);
  assert_eq(0, strcmp("a_name", m.testcase_append.args.arg1));
}

test(extract_classify_unit_test_correctly) {
  char teststring[14];
  strcpy(teststring, "test(a_name)");
  m.strstr.func = strstr;
  m.strlen.func = strlen;

  m.index_of.retval = teststring + strlen(teststring) - 1;

  extract((testcase_list_t*)0x1234, teststring);

  assert_eq(TESTCASE_IS_UNIT_TEST, m.testcase_append.args.arg2);
}

test(extract_classify_module_test_correctly) {
  char teststring[24];
  strcpy(teststring, "module_test(a_name)");
  m.strstr.func = strstr;
  m.strlen.func = strlen;

  m.index_of.retval = teststring + strlen(teststring) - 1;

  extract((testcase_list_t*)0x1234, teststring);

  assert_eq(TESTCASE_IS_MODULE_TEST, m.testcase_append.args.arg2);
}

/***************************************************************************
 * parse()
 */
test(parse_shall_retorn_0_on_success) {
  file_t file;
  file.len = 0;
  char buf[1];

  m.malloc.retval = buf;

  assert_eq(0, parse(NULL, &file));
}

test(parse_shall_call_malloc_for_allocating_a_buff) {
  file_t file;
  file.len = 5678;

  parse(NULL, &file);

  assert_eq(1, m.malloc.call_count);
}

test(parse_shall_allocate_the_correct_number_of_bytes_plus_one_for_null) {
  file_t file;
  file.len = 5678;

  parse(NULL, &file);

  assert_eq(5678 + 1, m.malloc.args.arg0);
}

test(parse_shall_return_negative_1_if_out_of_memory) {
  file_t file;
  file.len = 5678;

  m.malloc.retval = NULL;

  assert_eq(-1, parse(NULL, &file));
}

test(parse_shall_free_the_allocated_buffer_if_not_out_of_memory) {
  file_t file;
  char buf[1];

  file.len = 0;

  m.malloc.retval = buf;

  assert_eq(0, parse(NULL, &file));
}

#define quick_setup                             \
  file_t file;                                  \
  char buf[15];                                 \
  file.len = strlen(teststring);                \
  file.buf = (char*)teststring;                 \
  m.malloc.retval = buf;                        \
  parse(NULL, &file)

test(parse_shall_call_extract_if_end_of_line_is_found) {
  const char* teststring = "something\n";

  quick_setup;

  assert_eq(1, m.extract.call_count);
}

test(parse_shall_call_extract_if_code_block_start_is_found) {
  const char* teststring = "something{";

  quick_setup;

  assert_eq(1, m.extract.call_count);
}

test(parse_shall_not_call_extract_if_eol_found_in_block_comment) {
  const char* teststring = "/* something\n";

  quick_setup;

  assert_eq(0, m.extract.call_count);
}

test(parse_shall_not_call_extract_if_eol_found_in_row_comment) {
  const char* teststring = "// something\n";

  quick_setup;

  assert_eq(0, m.extract.call_count);
}

#undef quick_setup

/***************************************************************************
 * testcase_list_init()
 */
test(testcase_list_init_shall_call_parse_correctly) {
  testcase_list_init((testcase_list_t*)0x1234, (file_t*)0x5678);

  assert_eq(1, m.parse.call_count);
  assert_eq((testcase_list_t*)0x1234, m.parse.args.arg0);
  assert_eq((file_t*)0x5678, m.parse.args.arg1);
}
