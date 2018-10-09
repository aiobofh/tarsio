#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "testcase.h"
#include "testcase_data.h"
#include "tarsio.h"

#define m tarsio_mock

/***************************************************************************
 * testcase_list_append()
 */
test(list_append_shall_set_first_node_if_it_is_first_node) {
  testcase_list_t list = {NULL, NULL};
  testcase_list_append(&list, (testcase_node_t*)0x1234);
  assert_eq((testcase_node_t*)0x1234, list.first);
}

test(list_append_shall_not_set_first_node_if_it_is_not_first) {
  testcase_list_t list = {(testcase_node_t*)0x5678, NULL};
  testcase_list_append(&list, (testcase_node_t*)0x1234);
  assert_eq((testcase_node_t*)0x5678, list.first);
}

test(list_append_shall_set_last_always) {
  testcase_list_t list = {(testcase_node_t*)0x5678, NULL};
  testcase_list_append(&list, (testcase_node_t*)0x1234);
  assert_eq((testcase_node_t*)0x1234, list.last);
}

test(list_append_shall_set_previous_last_node_always) {
  testcase_node_t last = {NULL, TESTCASE_IS_UNKNOWN, NULL};
  testcase_list_t list = {(testcase_node_t*)0x5678, NULL};
  list.last = &last;
  testcase_list_append(&list, (testcase_node_t*)0x1234);
  assert_eq((testcase_node_t*)0x1234, last.next);
}

/***************************************************************************
 * new_node()
 */
test(new_node_shall_allocate_correct_ammount_of_memory) {
  new_node(NULL, TESTCASE_IS_UNKNOWN);
  assert_eq(1, m.malloc.call_count);
  assert_eq(sizeof(testcase_node_t), m.malloc.args.arg0);
}

test(new_node_shall_return_NULL_i_out_of_memory) {
  assert_eq(NULL, new_node(NULL, TESTCASE_IS_UNKNOWN));
}

test(new_node_shall_zero_allocated_memory) {
  testcase_node_t node;
  m.malloc.retval = (void*)&node;
  new_node(NULL, TESTCASE_IS_UNKNOWN);
#ifdef SASC
  assert_eq(1, m.__builtin_memset.call_count);
  assert_eq((void*)&node, m.__builtin_memset.args.arg0);
  assert_eq(0, m.__builtin_memset.args.arg1);
  assert_eq(sizeof(testcase_node_t), m.__builtin_memset.args.arg2);
#else
  assert_eq(1, m.memset.call_count);
  assert_eq((void*)&node, m.memset.args.arg0);
  assert_eq(0, m.memset.args.arg1);
  assert_eq(sizeof(testcase_node_t), m.memset.args.arg2);
#endif
}

test(new_node_shall_set_contents_correctly) {
  testcase_node_t node;
  m.malloc.retval = (void*)&node;
  new_node((char*)0x1234, TESTCASE_IS_MODULE_TEST);
  assert_eq((char*)0x1234, node.name);
  assert_eq(TESTCASE_IS_MODULE_TEST, node.type);
}

/***************************************************************************
 * print_len_warning()
 */
test(print_len_warning_shall_print_to_stderr) {
  print_len_warning(NULL);
  assert_eq(1, m.fprintf.call_count);
  assert_eq(stderr, m.fprintf.args.arg0);
}

/***************************************************************************
 * strclone()
 */
test(strclone_shall_warn_if_long_testcase_name) {
#ifdef SASC
  m.__builtin_strlen.retval = 80 + 1;
#else
  m.strlen.retval = 80 + 1;
#endif
  strclone(NULL);
  assert_eq(1, m.print_len_warning.call_count);
}

test(strclone_shall_malloc_enough_memory_for_string_copy) {
#ifdef SASC
  m.__builtin_strlen.func = strlen;
#else
  m.strlen.func = strlen;
#endif
  strclone("012345");
  assert_eq(1, m.malloc.call_count);
  assert_eq(7, m.malloc.args.arg0);
}

test(srtclone_shall_return_NULL_if_out_of_memory) {
  assert_eq(NULL, strclone("012345"));
}

test(strclone_shall_call_strcpy_correctly) {
  m.malloc.retval = (void*)0x1234;
  strclone((const char*)0x5678);
#ifdef SASC
  assert_eq(1, m.__builtin_strcpy.call_count);
  assert_eq((char*)0x1234, m.__builtin_strcpy.args.arg0);
  assert_eq((char*)0x5678, m.__builtin_strcpy.args.arg1);
#else
  assert_eq(1, m.strcpy.call_count);
  assert_eq((char*)0x1234, m.strcpy.args.arg0);
  assert_eq((char*)0x5678, m.strcpy.args.arg1);
#endif
}

test(strclone_shall_free_memory_if_strcpy_failed) {
  m.malloc.retval = (void*)0x1234;
  strclone((const char*)0x5678);
  assert_eq(1, m.free.call_count);
  assert_eq((void*)0x1234, m.free.args.arg0);
}

test(strclone_shall_not_free_memory_on_success) {
  m.malloc.retval = (void*)0x1234;
#ifdef SASC
  m.__builtin_strcpy.retval = m.malloc.retval;
#else
  m.strcpy.retval = m.malloc.retval;
#endif
  strclone((const char*)0x5678);
  assert_eq(0, m.free.call_count);
}

test(strclone_shall_return_the_new_string_pointer_on_success) {
  m.malloc.retval = (void*)0x1234;
#ifdef SASC
  m.__builtin_strcpy.retval = m.malloc.retval;
#else
  m.strcpy.retval = m.malloc.retval;
#endif
  assert_eq((char*)0x1234, strclone((const char*)0x5678));
}

/***************************************************************************
 * testcase_append()
 */
test(1_testcase_append_shall_call_strclone_with_name_as_argument) {
  testcase_append(NULL, (const char*)0x1234, TESTCASE_IS_UNKNOWN);
  assert_eq(1, m.strclone.call_count);
  assert_eq((const char*)0x1234, m.strclone.args.arg0);
}

test(2_testcase_append_shall_return_negative_1_if_name_could_not_be_cloned) {
  assert_eq(-1, testcase_append(NULL, NULL, TESTCASE_IS_UNKNOWN));
}

test(3_testcase_append_shall_call_new_node_correctly) {
  m.strclone.retval = (char*)0x1234;
  testcase_append(NULL, NULL, TESTCASE_IS_UNKNOWN);
  assert_eq(1, m.new_node.call_count);
  assert_eq((char*)0x1234, m.new_node.args.arg0);
  assert_eq(TESTCASE_IS_UNKNOWN, m.new_node.args.arg1);
}

test(4_testcase_append_shall_return_negative_2_if_new_node_failed) {
  m.strclone.retval = (char*)0x1234;
  assert_eq(-2, testcase_append(NULL, NULL, TESTCASE_IS_UNKNOWN));
}

test(5_testcase_append_shall_call_testcase_list_append_correctly) {
  m.strclone.retval = (char*)0x1234;
  m.new_node.retval = (testcase_node_t*)0x5678;
  testcase_append((testcase_list_t*)0x4321, NULL, TESTCASE_IS_UNKNOWN);
  assert_eq(1, m.testcase_list_append.call_count);
  assert_eq((testcase_list_t*)0x4321, m.testcase_list_append.args.arg0);
  assert_eq((testcase_node_t*)0x5678, m.testcase_list_append.args.arg1);
}

/***************************************************************************
 * index_of()
 */
test(index_of_shall_return_the_pointer_to_the_position_of_a_specific_char) {
  char* teststring = "12345";
  (void)m;
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

#ifdef SASC
#define quick_setup				\
  m.strstr.func = strstr;			\
  m.__builtin_strlen.func = strlen
#else
#define quick_setup				\
  m.strstr.func = strstr;			\
  m.strlen.func = strlen
#endif

test(extract_search_for_en_parentheis_using_index_of) {
  quick_setup;

  extract(NULL, "test(a_name)");

  assert_eq(1, m.index_of.call_count);
  assert_eq(0, strcmp("a_name)", m.index_of.args.arg0));
  assert_eq(')', m.index_of.args.arg1);
}

test(extract_return_negative_1_if_parentheis_was_not_found) {
  quick_setup;

  m.index_of.retval = NULL;

  assert_eq(-1, extract(NULL, "test(a_name)"));
}

test(extract_not_append_test_case_if_name_not_found) {
  quick_setup;

  m.index_of.retval = NULL;

  extract(NULL, "test(a_name)");

  assert_eq(0, m.testcase_append.call_count);
}

test(extract_append_test_case_if_name_not_found) {
  char teststring[14];
  strcpy(teststring, "test(a_name)");
  quick_setup;

  m.index_of.retval = teststring + strlen("test(");

  extract(NULL, teststring);

  assert_eq(1, m.testcase_append.call_count);
}

test(extract_call_append_test_correctly) {
  char teststring[14];
  strcpy(teststring, "test(a_name)");
  quick_setup;

  m.index_of.retval = teststring + strlen(teststring) - 1;

  extract((testcase_list_t*)0x1234, teststring);

  assert_eq(1, m.testcase_append.call_count);
  assert_eq((testcase_list_t*)0x1234, m.testcase_append.args.arg0);
  assert_eq(0, strcmp("a_name", m.testcase_append.args.arg1));
}

test(extract_classify_unit_test_correctly) {
  char teststring[14];
  strcpy(teststring, "test(a_name)");
  quick_setup;

  m.index_of.retval = teststring + strlen(teststring) - 1;

  extract((testcase_list_t*)0x1234, teststring);

  assert_eq(TESTCASE_IS_UNIT_TEST, m.testcase_append.args.arg2);
}

test(extract_classify_module_test_correctly) {
  char teststring[24];
  strcpy(teststring, "module_test(a_name)");
  quick_setup;

  m.index_of.retval = teststring + strlen(teststring) - 1;

  extract((testcase_list_t*)0x1234, teststring);

  assert_eq(TESTCASE_IS_MODULE_TEST, m.testcase_append.args.arg2);
}

test(extract_shall_return_negative_2_if_testcase_append_failed) {
  char teststring[24];
  strcpy(teststring, "module_test(a_name)");
  quick_setup;

  m.index_of.retval = teststring + strlen(teststring) - 1;
  m.testcase_append.retval = -1;

  assert_eq(-2, extract((testcase_list_t*)0x1234, teststring));
}

#undef quick_setup



/***************************************************************************
 * parse()
 */
test(parse_retorn_0_on_success) {
  file_t file;
  char buf[1];
  file.len = 0;

  m.malloc.retval = buf;

  assert_eq(0, parse(NULL, &file));
}

test(parse_malloc_for_allocating_a_buff) {
  file_t file;
  file.len = 5678;

  parse(NULL, &file);

  assert_eq(1, m.malloc.call_count);
}

test(parse_allocate_the_correct_number_of_bytes_plus_one_for_null) {
  file_t file;
  file.len = 5678;

  parse(NULL, &file);

  assert_eq(5678 + 1, m.malloc.args.arg0);
}

test(parse_return_negative_1_if_out_of_memory) {
  file_t file;
  file.len = 5678;

  m.malloc.retval = NULL;

  assert_eq(-1, parse(NULL, &file));
}

test(parse_free_the_allocated_buffer_if_not_out_of_memory) {
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

test(parse_extract_if_end_of_line_is_found) {
  const char* teststring = "something\n";

  quick_setup;

  assert_eq(1, m.extract.call_count);
}

test(parse_extract_if_code_block_start_is_found) {
  const char* teststring = "something{";

  quick_setup;

  assert_eq(1, m.extract.call_count);
}

test(parse_not_extract_if_eol_in_block_comment) {
  const char* teststring = "/* something\n";

  quick_setup;

  assert_eq(0, m.extract.call_count);
}

test(parse_not_extract_if_eol_in_row_comment) {
  const char* teststring = "// something\n";

  quick_setup;

  assert_eq(0, m.extract.call_count);
}

#undef quick_setup

test(parse_return_negative_2_if_extract_fail) {
  const char* teststring = "something;\n";
  file_t file;
  char buf[15];
  file.len = strlen(teststring);
  file.buf = (char*)teststring;
  m.malloc.retval = buf;
  m.extract.retval = -1;
  assert_eq(-2, parse(NULL, &file));
}

/***************************************************************************
 * testcase_list_init()
 */
test(testcase_list_init_list_NULL_assert) {
  testcase_list_init(NULL, (file_t*)0x1234);
#ifdef SASC
  assert_eq(2, m.__assert.call_count);
#else
  assert_eq(1, m.__assert_fail.call_count);
#endif
}

test(testcase_list_init_file_NULL_assert) {
  testcase_list_init((testcase_list_t*)0x1234, NULL);
#ifdef SASC
  assert_eq(2, m.__assert.call_count);
#else
  assert_eq(1, m.__assert_fail.call_count);
#endif
}

test(testcase_list_init_shall_call_parse_correctly) {
  testcase_list_init((testcase_list_t*)0x1234, (file_t*)0x5678);

  assert_eq(1, m.parse.call_count);
  assert_eq((testcase_list_t*)0x1234, m.parse.args.arg0);
  assert_eq((file_t*)0x5678, m.parse.args.arg1);
}

/***************************************************************************
 * testcase_node_cleanup()
 */

test(node_cleanup_shall_free_name_and_nodede) {
  testcase_node_t node;
  node.name = (char*)0x1234;
  testcase_node_cleanup(&node);
  assert_eq(2, m.free.call_count);
}

test(node_cleanup_shall_free_node) {
  testcase_node_t node;
  node.name = (char*)NULL;
  testcase_node_cleanup(&node);
  assert_eq(1, m.free.call_count);
}

/***************************************************************************
 * testcase_node_cleanup()
 */
test(testcase_list_cleanup_shall_clean_all_nodes) {
  testcase_list_t list;
  testcase_node_t node[3];
  list.first = &node[0];
  list.last = &node[2];
  node[0].next = &node[1];
  node[1].next = &node[2];
  node[2].next = NULL;

  testcase_list_cleanup(&list);

  assert_eq(3, m.testcase_node_cleanup.call_count);
}
