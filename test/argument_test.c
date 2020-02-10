#include "argument.h"
#include "argument_data.h"
#include "tarsio.h"

#include "helpers.h"

#define m tarsio_mock

test(new_shall_malloc_the_correct_size) {
  argument_node_new(NULL, NULL, 0, 0, 0);
  assert_eq(1, m.malloc.call_count);
  assert_eq(sizeof(argument_node_t), m.malloc.args.arg0);
}

test(new_shall_return_NULL_if_out_of_memory) {
  m.malloc.retval = NULL;
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
  assert_eq(1, m.MEMSET.call_count);
  assert_eq(&node, m.MEMSET.args.arg0);
  assert_eq(0, m.MEMSET.args.arg1);
  assert_eq(sizeof(node), m.MEMSET.args.arg2);
}

test(append_shall_call_list_append) {
  argument_list_append((argument_list_t*)0x1234,
                       (argument_node_t*)0x5678);
  assert_eq(1, m.base_list_append.call_count);
  assert_eq((list_t*)0x1234, m.base_list_append.args.arg0);
  assert_eq((node_t*)0x5678, m.base_list_append.args.arg1);
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

test(node_cleanup_should_assert_on_NULL_arg) {
  argument_list_cleanup(NULL);
  assert_eq(1, m.ASSERT.call_count);
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
