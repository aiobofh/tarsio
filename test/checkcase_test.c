/*
 * Check-suite for ../src/checkcase.c
 *
 *              _______          _____ ___        ______
 *                 |      ||    |         |    | |      |
 *                 |      ||    |         |    | |      |
 *                 |   ___||___ |         |___ | |______|
 *
 *              Copyright (C) 2020 AiO Secure Teletronics
 *
 *  This file is part of Tarsio.
 *
 *  Tarsio is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Tarsio is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Tarsio.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "checkcase.h"
#include "checkcase_data.h"
#include "tarsio.h"
#include "helpers.h"

#define m tarsio_mock

/***************************************************************************
 * checkcase_list_append()
 */
check(list_append_shall_set_first_node_if_it_is_first_node) {
  checkcase_list_t list = {NULL, NULL};
  checkcase_list_append(&list, (checkcase_node_t*)0x1234);
  assert_eq((checkcase_node_t*)0x1234, list.first);
}

check(list_append_shall_not_set_first_node_if_it_is_not_first) {
  checkcase_list_t list = {(checkcase_node_t*)0x5678, NULL};
  checkcase_list_append(&list, (checkcase_node_t*)0x1234);
  assert_eq((checkcase_node_t*)0x5678, list.first);
}

check(list_append_shall_set_last_always) {
  checkcase_list_t list = {(checkcase_node_t*)0x5678, NULL};
  checkcase_list_append(&list, (checkcase_node_t*)0x1234);
  assert_eq((checkcase_node_t*)0x1234, list.last);
}

check(list_append_shall_set_previous_last_node_always) {
  checkcase_node_t last = {NULL, CHECKCASE_IS_UNKNOWN, NULL};
  checkcase_list_t list = {(checkcase_node_t*)0x5678, NULL};
  list.last = &last;
  checkcase_list_append(&list, (checkcase_node_t*)0x1234);
  assert_eq((checkcase_node_t*)0x1234, last.next);
}

/***************************************************************************
 * new_node()
 */
check(new_node_shall_allocate_correct_ammount_of_memory) {
  new_node(NULL, CHECKCASE_IS_UNKNOWN);
  assert_eq(1, m.malloc.call_count);
  assert_eq(sizeof(checkcase_node_t), m.malloc.args.arg0);
}

check(new_node_shall_return_NULL_i_out_of_memory) {
  assert_eq(NULL, new_node(NULL, CHECKCASE_IS_UNKNOWN));
}

check(new_node_shall_zero_allocated_memory) {
  checkcase_node_t node;
  m.malloc.retval = (void*)&node;
  new_node(NULL, CHECKCASE_IS_UNKNOWN);
  assert_eq(1, m.MEMSET.call_count);
  assert_eq((void*)&node, m.MEMSET.args.arg0);
  assert_eq(0, m.MEMSET.args.arg1);
  assert_eq(sizeof(checkcase_node_t), m.MEMSET.args.arg2);
}

check(new_node_shall_set_contents_correctly) {
  checkcase_node_t node;
  m.malloc.retval = (void*)&node;
  new_node((char*)0x1234, CHECKCASE_IS_MODULE_CHECK);
  assert_eq((char*)0x1234, node.name);
  assert_eq(CHECKCASE_IS_MODULE_CHECK, node.type);
}

/***************************************************************************
 * print_len_warning()
 */
check(print_len_warning_shall_print_to_stderr) {
  print_len_warning(NULL);
  assert_eq(1, m.fprintf.call_count);
  assert_eq(stderr, m.fprintf.args.arg0);
}

/***************************************************************************
 * checkcase_append()
 */
check(0_checkcase_append_shall_warn_if_long_checkcase_name) {
  m.STRLEN.retval = 80 + 1;
  checkcase_append(NULL, (const char*)0x1234, CHECKCASE_IS_UNKNOWN);
  assert_eq(1, m.print_len_warning.call_count);
}

check(1_checkcase_append_shall_call_strclone_with_name_as_argument) {
  checkcase_append(NULL, (const char*)0x1234, CHECKCASE_IS_UNKNOWN);
  assert_eq(1, m.strclone.call_count);
  assert_eq((const char*)0x1234, m.strclone.args.arg0);
}

check(2_checkcase_append_shall_return_negative_1_if_name_could_not_be_cloned) {
  assert_eq(-1, checkcase_append(NULL, NULL, CHECKCASE_IS_UNKNOWN));
}

check(3_checkcase_append_shall_call_new_node_correctly) {
  m.strclone.retval = (char*)0x1234;
  checkcase_append(NULL, NULL, CHECKCASE_IS_UNKNOWN);
  assert_eq(1, m.new_node.call_count);
  assert_eq((char*)0x1234, m.new_node.args.arg0);
  assert_eq(CHECKCASE_IS_UNKNOWN, m.new_node.args.arg1);
}

check(4_checkcase_append_shall_return_negative_2_if_new_node_failed) {
  m.strclone.retval = (char*)0x1234;
  assert_eq(-2, checkcase_append(NULL, NULL, CHECKCASE_IS_UNKNOWN));
}

check(5_checkcase_append_shall_call_checkcase_list_append_correctly) {
  m.strclone.retval = (char*)0x1234;
  m.new_node.retval = (checkcase_node_t*)0x5678;
  checkcase_append((checkcase_list_t*)0x4321, NULL, CHECKCASE_IS_UNKNOWN);
  assert_eq(1, m.checkcase_list_append.call_count);
  assert_eq((checkcase_list_t*)0x4321, m.checkcase_list_append.args.arg0);
  assert_eq((checkcase_node_t*)0x5678, m.checkcase_list_append.args.arg1);
}

/***************************************************************************
 * index_of()
 */
check(index_of_shall_return_the_pointer_to_the_position_of_a_specific_char) {
  char* checkstring = "12345";
#ifndef VBCC
  (void)m;
#endif
  assert_eq(&checkstring[4], index_of(checkstring, '5'));
}

/***************************************************************************
 * extract()
 */
check(extract_return_0_on_success) {
#ifndef VBCC
  (void)m;
#endif
  assert_eq(0, extract(NULL, NULL));
}

#define quick_setup  m.strstr.func = strstr; m.STRLEN.func = strlen

check(extract_search_for_end_parentheis_using_index_of) {
  quick_setup;

  extract(NULL, "check(a_name)");

  assert_eq(1, m.index_of.call_count);
  assert_eq(0, strcmp("a_name)", m.index_of.args.arg0));
  assert_eq(')', m.index_of.args.arg1);
}

check(extract_return_negative_1_if_parentheis_was_not_found) {
  quick_setup;

  m.index_of.retval = NULL;

  assert_eq(-1, extract(NULL, "check(a_name)"));
}

check(extract_not_append_check_case_if_name_not_found) {
  quick_setup;

  m.index_of.retval = NULL;

  extract(NULL, "check(a_name)");

  assert_eq(0, m.checkcase_append.call_count);
}

check(extract_append_check_case_if_name_not_found) {
  char checkstring[14];
  strcpy(checkstring, "check(a_name)");
  quick_setup;

  m.index_of.retval = checkstring + strlen("check(");

  extract(NULL, checkstring);

  assert_eq(1, m.checkcase_append.call_count);
}

check(extract_call_append_check_correctly) {
  char checkstring[14];
  strcpy(checkstring, "check(a_name)");
  quick_setup;

  m.index_of.retval = checkstring + strlen(checkstring) - 1;

  extract((checkcase_list_t*)0x1234, checkstring);

  assert_eq(1, m.checkcase_append.call_count);
  assert_eq((checkcase_list_t*)0x1234, m.checkcase_append.args.arg0);
  assert_eq(0, strcmp("a_name", m.checkcase_append.args.arg1));
}

check(extract_classify_unit_check_correctly) {
  char checkstring[14];
  strcpy(checkstring, "check(a_name)");
  quick_setup;

  m.index_of.retval = checkstring + strlen(checkstring) - 1;

  extract((checkcase_list_t*)0x1234, checkstring);

  assert_eq(CHECKCASE_IS_UNIT_CHECK, m.checkcase_append.args.arg2);
}

check(extract_classify_module_check_correctly) {
  char checkstring[24];
  strcpy(checkstring, "module_check(a_name)");
  quick_setup;

  m.index_of.retval = checkstring + strlen(checkstring) - 1;

  extract((checkcase_list_t*)0x1234, checkstring);

  assert_eq(CHECKCASE_IS_MODULE_CHECK, m.checkcase_append.args.arg2);
}

check(extract_shall_return_negative_2_if_checkcase_append_failed) {
  char checkstring[24];
  strcpy(checkstring, "module_check(a_name)");
  quick_setup;

  m.index_of.retval = checkstring + strlen(checkstring) - 1;
  m.checkcase_append.retval = -1;

  assert_eq(-2, extract((checkcase_list_t*)0x1234, checkstring));
}

#undef quick_setup



/***************************************************************************
 * parse()
 */
check(parse_retorn_0_on_success) {
  file_t file;
  char buf[1];
  file.len = 0;

  m.malloc.retval = buf;

  assert_eq(0, parse(NULL, &file));
}

check(parse_malloc_for_allocating_a_buff) {
  file_t file;
  file.len = 5678;

  parse(NULL, &file);

  assert_eq(1, m.malloc.call_count);
}

check(parse_allocate_the_correct_number_of_bytes_plus_one_for_null) {
  file_t file;
  file.len = 5678;

  parse(NULL, &file);

  assert_eq(5678 + 1, m.malloc.args.arg0);
}

check(parse_return_negative_1_if_out_of_memory) {
  file_t file;
  file.len = 5678;

  m.malloc.retval = NULL;

  assert_eq(-1, parse(NULL, &file));
}

check(parse_free_the_allocated_buffer_if_not_out_of_memory) {
  file_t file;
  char buf[1];

  file.len = 0;

  m.malloc.retval = buf;

  assert_eq(0, parse(NULL, &file));
}

#define quick_setup_len(LEN)                    \
  file_t file;                                  \
  char buf[15];                                 \
  file.len = LEN;                               \
  file.buf = (char*)checkstring;                \
  m.malloc.retval = buf

extern void __tarsio_proxy_printint(const char* str, const int val);

check(parse_extract_if_end_of_line_is_found) {
  const char* checkstring = "something\n";
  const size_t len = 11; /* Don't know why strlen wont work with vbcc here */

  quick_setup_len(len);
  parse(NULL, &file);

  assert_eq(1, m.extract.call_count);
}

check(parse_extract_if_code_block_start_is_found) {
  const char* checkstring = "something{";
  const size_t len = 10; /* Don't know why strlen wont work with vbcc here */

  quick_setup_len(len);
  parse(NULL, &file);

  assert_eq(1, m.extract.call_count);
}

check(parse_not_extract_if_eol_in_block_comment) {
  const char* checkstring = "/* something\n";
  const size_t len = 14; /* Don't know why strlen wont work with vbcc here */

  quick_setup_len(len);
  parse(NULL, &file);

  assert_eq(0, m.extract.call_count);
}

check(parse_not_extract_if_eol_in_row_comment) {
  const char* checkstring = "// something\n";
  const size_t len = 14; /* Don't know why strlen wont work with vbcc here */

  quick_setup_len(len);
  parse(NULL, &file);

  assert_eq(0, m.extract.call_count);
}

#undef quick_setup_len

check(parse_return_negative_2_if_extract_fail) {
  const char* checkstring = "something;\n";
  file_t file;
  char buf[15];
  file.len = 12;
  file.buf = (char*)checkstring;
  m.malloc.retval = buf;
  m.extract.retval = -1;
  assert_eq(-2, parse(NULL, &file));
}

/***************************************************************************
 * checkcase_list_init()
 */
check(checkcase_list_init_list_NULL_assert) {
  checkcase_list_init(NULL, (file_t*)0x1234);
#ifndef SASC
  assert_eq(1, m.ASSERT.call_count);
#else
  skip("Not applicable on SASC");
#endif
}

check(checkcase_list_init_file_NULL_assert) {
  checkcase_list_init((checkcase_list_t*)0x1234, NULL);
#ifndef SASC
  assert_eq(1, m.ASSERT.call_count);
#else
  skip("Not applicable on SASC");
#endif
}

check(checkcase_list_init_shall_call_parse_correctly) {
  checkcase_list_init((checkcase_list_t*)0x1234, (file_t*)0x5678);

  assert_eq(1, m.parse.call_count);
  assert_eq((checkcase_list_t*)0x1234, m.parse.args.arg0);
  assert_eq((file_t*)0x5678, m.parse.args.arg1);
}

/***************************************************************************
 * checkcase_node_cleanup()
 */

check(node_cleanup_shall_free_name_and_nodede) {
  checkcase_node_t node;
  node.name = (char*)0x1234;
  checkcase_node_cleanup(&node);
  assert_eq(2, m.free.call_count);
}

check(node_cleanup_shall_free_node) {
  checkcase_node_t node;
  node.name = (char*)NULL;
  checkcase_node_cleanup(&node);
  assert_eq(1, m.free.call_count);
}

/***************************************************************************
 * checkcase_node_cleanup()
 */
check(checkcase_list_cleanup_shall_clean_all_nodes) {
  checkcase_list_t list;
  checkcase_node_t node[3];
  list.first = &node[0];
  list.last = &node[2];
  node[0].next = &node[1];
  node[1].next = &node[2];
  node[2].next = NULL;

  checkcase_list_cleanup(&list);

  assert_eq(3, m.checkcase_node_cleanup.call_count);
}
