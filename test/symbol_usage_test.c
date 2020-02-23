/*
 * Check-suite for ../src/symbol_usage.c
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

#include "symbol_usage.h"
#include "symbol_usage_data.h"
#include "tarsio.h"
#include "helpers.h"

#define m tarsio_mock

/***************************************************************************
 * symbol_usage_node_new()
 */
check(new_shall_malloc_the_correct_size) {
  symbol_usage_node_new(0, 0, 0, 0, 0, NULL);
  assert_eq(1, m.malloc.call_count);
  assert_eq(sizeof(symbol_usage_node_t), m.malloc.args.arg0);
}

check(new_shall_return_NULL_if_out_of_memory) {
  assert_eq(NULL, symbol_usage_node_new(0, 0, 0, 0, 0, NULL));
}

check(new_shall_return_the_new_node_if_ok) {
  symbol_usage_node_t node;
  m.malloc.retval = &node;
  assert_eq(&node, symbol_usage_node_new(0, 0, 0, 0, 0, NULL));
}

check(new_shall_clear_the_whole_node) {
  symbol_usage_node_t node;
  m.malloc.retval = &node;
  symbol_usage_node_new(0, 0, 0, 0, 0, NULL);
  assert_eq(1, m.MEMSET.call_count);
  assert_eq(&node, m.MEMSET.args.arg0);
  assert_eq(0, m.MEMSET.args.arg1);
  assert_eq(sizeof(node), m.MEMSET.args.arg2);
}

check(new_shall_set_the_line_col_and_offset) {
  symbol_usage_node_t node;
  m.malloc.retval = &node;
  symbol_usage_node_new(1, 2, 3, 0, 0, NULL);
  assert_eq(1, node.info.line);
  assert_eq(2, node.info.col);
  assert_eq(3, node.info.offset);
}

/***************************************************************************
 * symbol_usage_append()
 */
check(append_shall_not_append_if_out_of_memory) {
  symbol_usage_list_t list = SYMBOL_USAGE_LIST_EMPTY;
  m.symbol_usage_node_new.retval = NULL;
  symbol_usage_append(&list, 0, 0, 0, 0, 0, NULL);
  assert_eq(NULL, list.first);
  assert_eq(NULL, list.last);
}

check(append_shall_add_node_to_empty_list) {
  symbol_usage_node_t node;
  symbol_usage_list_t list = SYMBOL_USAGE_LIST_EMPTY;
  m.symbol_usage_node_new.retval = &node;
  symbol_usage_append(&list, 0, 0, 0, 0, 0, NULL);
  assert_eq(&node, list.first);
  assert_eq(&node, list.last);
}

check(append_shall_add_node_to_end_of_list) {
  symbol_usage_node_t node1;
  symbol_usage_node_t node2;
  symbol_usage_list_t list = SYMBOL_USAGE_LIST_EMPTY;
  list.first = &node1;
  list.last = &node1;
  m.symbol_usage_node_new.retval = &node2;
  symbol_usage_append(&list, 0, 0, 0, 0, 0, NULL);
  assert_eq(&node1, list.first);
  assert_eq(&node2, list.last);
  assert_eq(node1.next, &node2);
}

check(append_shall_increment_node_count_on_success) {
  symbol_usage_node_t node;
  symbol_usage_list_t list = SYMBOL_USAGE_LIST_EMPTY;
  m.symbol_usage_node_new.retval = &node;
  symbol_usage_append(&list, 0, 0, 0, 0, 0, NULL);
  assert_eq(1, list.cnt);
}

/***************************************************************************
 * symbol_usage_list_cleanup()
 */
check(cleanup_shall_free_all_nodes) {
  symbol_usage_node_t node1 = SYMBOL_USAGE_NODE_EMPTY;
  symbol_usage_node_t node2 = SYMBOL_USAGE_NODE_EMPTY;
  symbol_usage_list_t list = SYMBOL_USAGE_LIST_EMPTY;
  list.first = &node1;
  list.last = &node2;
  node1.next = &node2;
  symbol_usage_list_cleanup(&list);
  assert_eq(2, m.free.call_count);
}
