#include "argument.h"
#include "argument_data.h"
#include "tarsio.h"

#define m tarsio_mock

test(new_shall_malloc_the_correct_size) {
  argument_node_new(NULL, NULL, 0, 0, 0);
  assert_eq(1, m.malloc.call_count);
  assert_eq(sizeof(argument_node_t), m.malloc.args.arg0);
}

test(new_shall_return_NULL_if_out_of_memory) {
  assert_eq(NULL, argument_node_new(NULL, NULL, 0, 0, 0));
}

test(new_shall_return_the_new_node_if_ok) {
  argument_node_t node;
  m.malloc.retval = &node;
  assert_eq(&node, argument_node_new(NULL, NULL, 0, 0, 0));
}

test(new_shall_clear_the_whole_node) {
  argument_node_t node;
  m.malloc.retval = &node;
  argument_node_new(NULL, NULL, 0, 0, 0);
  assert_eq(1, m.memset.call_count);
  assert_eq(&node, m.memset.args.arg0);
  assert_eq(0, m.memset.args.arg1);
  assert_eq(sizeof(node), m.memset.args.arg2);
}

test(append_shall_add_node_to_empty_list) {
  argument_node_t node;
  argument_list_t list = ARGUMENT_LIST_EMPTY;
  argument_list_append(&list, &node);
  assert_eq(&node, list.first);
  assert_eq(&node, list.last);
}

test(append_shall_add_node_to_end_of_list) {
  argument_node_t node1;
  argument_node_t node2;
  argument_list_t list = ARGUMENT_LIST_EMPTY;
  list.first = &node1;
  list.last = &node1;
  argument_list_append(&list, &node2);
  assert_eq(&node1, list.first);
  assert_eq(&node2, list.last);
  assert_eq(node1.next, &node2);
}

test(append_shall_increment_node_count_on_success) {
  argument_node_t node;
  argument_list_t list = ARGUMENT_LIST_EMPTY;
  argument_list_append(&list, &node);
  assert_eq(1, list.cnt);
}

test(cleanup_shall_cleanup_datatype) {
  argument_t argument;
  argument_cleanup(&argument);
  assert_eq(1, m.datatype_cleanup.call_count);
  assert_eq(&argument.datatype, m.datatype_cleanup.args.arg0);
}

test(cleanup_shall_free_name) {
  argument_t argument;
  argument.name = (char*)1234;
  argument_cleanup(&argument);
  assert_eq(1, m.free.call_count);
  assert_eq((void*)1234, m.free.args.arg0);
}

test(cleanup_shall_not_free_name_if_NULL) {
  argument_t argument;
  argument.name = NULL;
  argument_cleanup(&argument);
  assert_eq(0, m.free.call_count);
}

test(node_cleanup_shall_cleanup_argument_data) {
  argument_node_t node;
  argument_node_cleanup(&node);
  assert_eq(1, m.argument_cleanup.call_count);
  assert_eq(&node.info, m.argument_cleanup.args.arg0);
}

test(list_cleanup_shall_free_all_nodes) {
  argument_node_t node1 = ARGUMENT_NODE_EMPTY;
  argument_node_t node2 = ARGUMENT_NODE_EMPTY;
  argument_list_t list = ARGUMENT_LIST_EMPTY;
  list.first = &node1;
  list.last = &node2;
  node1.next = &node2;
  argument_list_cleanup(&list);
  assert_eq(2, m.free.call_count);
}
