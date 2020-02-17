/*
 * Check-suite for ../src/cpp.c
 *
 *              _______          _____ ___        ______
 *                 |      ||    |         |    | |      |
 *                 |      ||    |         |    | |      |
 *                 |   ___||___ |         |___ | |______|
 *
 *                   Copyleft AiO Secure Teletronics
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

#include "cpp.h"
#include "cpp_data.h"

#include "tarsio.h"

#include "helpers.h"

#define m tarsio_mock

/***************************************************************************
 * cpp_node_new()
 */
test(new_shall_allocate_enough_memeory) {
  cpp_node_new(NULL);
  assert_eq(1, m.malloc.call_count);
}

test(new_shall_return_NULL_if_out_of_memeory) {
  assert_eq(NULL, cpp_node_new(NULL));
}

test(new_shall_set_all_allocated_memory_to_zero) {
  cpp_node_t node;
  m.malloc.retval = (void*)&node;
  cpp_node_new(NULL);
  assert_eq(1, m.MEMSET.call_count);
  assert_eq((void*)&node, m.MEMSET.args.arg0);
  assert_eq(0, m.MEMSET.args.arg1);
  assert_eq(sizeof(cpp_node_t), m.MEMSET.args.arg2);
}

test(new_shall_clone_the_cpp_directive) {
  cpp_node_t node;
  m.malloc.retval = (void*)&node;
  cpp_node_new((char*)5678);
  assert_eq(1, m.strclone.call_count);
  assert_eq((char*)5678, m.strclone.args.arg0);
}

test(new_shall_free_the_node_if_strclone_fails) {
  cpp_node_t node;
  m.malloc.retval = (void*)&node;
  m.strclone.retval = NULL;
  cpp_node_new((char*)5678);
  assert_eq(1, m.free.call_count);
  assert_eq((char*)&node, m.free.args.arg0);
}

test(new_shall_return_NULL_if_strclone_fails) {
  cpp_node_t node;
  m.malloc.retval = (void*)&node;
  m.strclone.retval = NULL;
  assert_eq(NULL, cpp_node_new((char*)5678));
}

/***************************************************************************
 * cpp_list_append()
 */
test(append_should_create_a_new_cpp_node_correctly) {
  cpp_list_append((cpp_list_t*)0x1234, (const char*)0x5678);
  assert_eq(1, m.cpp_node_new.call_count);
  assert_eq((const char*)0x5678, m.cpp_node_new.args.arg0);
}

test(append_should_not_append_if_new_cpp_node_failed) {
  cpp_list_append((cpp_list_t*)0x1234, (const char*)0x5678);
  assert_eq(0, m.base_list_append.call_count);
}

test(append_should_append_if_new_cpp_node_was_ok) {
  m.cpp_node_new.retval = (cpp_node_t*)0x8765;
  cpp_list_append((cpp_list_t*)0x1234, (const char*)0x5678);
  assert_eq(1, m.base_list_append.call_count);
  assert_eq((list_t*)0x1234, m.base_list_append.args.arg0);
  assert_eq((node_t*)0x8765, m.base_list_append.args.arg1);
}

/***************************************************************************
 * cpp_list_init()
 */
test(list_init_shall_assert_list_is_not_NULL) {
  cpp_list_init(NULL, (file_t*)0x1234);
#ifndef SASC
  assert_eq(1, m.ASSERT.call_count);
#else
  skip("Not applicable on SASC");
#endif
}

test(list_init_shall_assert_file_is_not_NULL) {
  cpp_list_init((cpp_list_t*)0x1234, NULL);
#ifndef SASC
  assert_eq(1, m.ASSERT.call_count);
#else
  skip("Not applicable on SASC");
#endif
}

test(list_init_shall_set_up_the_file_parser_callback) {
  cpp_list_init((cpp_list_t*)0x1234, (file_t*)0x5678);
  assert_eq(1, m.file_parse.call_count);
  assert_eq((cpp_list_t*)0x1234, m.file_parse.args.arg1);
  assert_eq((file_t*)0x5678, m.file_parse.args.arg2);
}

/***************************************************************************
 * cpp_list_init()
 */
test(node_cleanup_shall_always_free_node) {
  cpp_node_t node;
  node.info.directive = NULL;
  cpp_node_cleanup(&node);
  assert_eq(1, m.free.call_count);
  assert_eq((void*)&node, m.free.args.arg0);
}

test(node_cleanup_shall_free_cpp_directive) {
  cpp_node_t node;
  node.info.directive = (char*)0x1234;
  cpp_node_cleanup(&node);
  assert_eq(2, m.free.call_count); /* Refactor this when arg memory is impl */
}

/***************************************************************************
 * cpp_list_cleanup()
 */
test(list_cleanup_shall_assert_if_provided_list_is_null) {
  cpp_list_cleanup(NULL);
#ifndef SASC
  assert_eq(1, m.ASSERT.call_count);
#else
  assert_eq(1, m.__assert.call_count);
#endif
}

test(list_cleanup_shall_clear_all_nodes) {
  cpp_node_t node1;
  cpp_node_t node2;
  cpp_list_t list;
  list.first = &node1;
  list.last = &node2;
  node1.next = &node2;
  node2.next = NULL;
  cpp_list_cleanup(&list);
  assert_eq(2, m.cpp_node_cleanup.call_count);
}
