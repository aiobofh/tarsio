#include "list.h"
#include "list_data.h"
#include "tarsio.h"

#define m tarsio_mock

test(append_shall_add_node_to_empty_list) {
  node_t node = NODE_EMPTY;
  list_t list = LIST_EMPTY;
  list_append(&list, &node);
  assert_eq(&node, list.first);
  assert_eq(&node, list.last);
}

test(append_shall_add_node_to_end_of_list) {
  node_t node1 = NODE_EMPTY;
  node_t node2 = NODE_EMPTY;
  list_t list = LIST_EMPTY;
  list.first = &node1;
  list.last = &node1;
  list_append(&list, &node2);
  assert_eq(&node1, list.first);
  assert_eq(&node2, list.last);
  assert_eq(node1.next, &node2);
}

test(append_shall_increment_node_count_on_success) {
  node_t node = NODE_EMPTY;
  list_t list = LIST_EMPTY;
  list_append(&list, &node);
  assert_eq(1, list.cnt);
}
