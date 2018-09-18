#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "debug.h"
#include "error.h"

#include "file.h"
#include "testcase.h"

static void testcase_list_append(testcase_list_t* list, testcase_node_t* node) {
  if (NULL == list->first) {
    list->first = node;
  }
  if (NULL != list->last) {
    list->last->next = node;
  }
  list->last = node;
}

static int testcase_append(testcase_list_t* list, const char* name, const testcase_type_t type) {
  int retval = 0;
  testcase_node_t* node;

  if (NULL == (node = malloc(sizeof(*node)))) {
    error1("Out of memory while allocating testcase node for '%s'", name);
    retval = -1;
    goto testcase_node_malloc_failed;
  }

  memset(node, 0, sizeof(*node));

  if (NULL == (node->name = malloc(strlen(name)+ 1))) {
    error1("Out of memory while allocating name '%s'", name);
    retval = -2;
    goto testcase_name_malloc_failed;
  }

  if (NULL == strcpy(node->name, name)) {
    error1("Could not copy testcase name '%s'", name);
    retval = -3;
    goto strcpy_failed;
  }
  node->type = type;

  testcase_list_append(list, node);

  goto normal_exit;

 strcpy_failed:
 testcase_name_malloc_failed:
  free(node);
 testcase_node_malloc_failed:
 normal_exit:
  return retval;
}

static char *index_of(const char* buf, char c) {
  while (*buf != c) {
    buf++;
  }
  return (char*)buf;
}

static int extract(testcase_list_t* list, char* buf) {
  int retval = 0;
  char* name = NULL;
  char* ptr;
  testcase_type_t type = TESTCASE_IS_UNKNOWN;

  if (buf == strstr(buf, "test(")) {
    name = buf + strlen("test(");
    type = TESTCASE_IS_UNIT_TEST;
  }
  else if (buf == strstr(buf, "module_test(")) {
    name = buf + strlen("module_test(");
    type = TESTCASE_IS_MODULE_TEST;
  }

  if (NULL == name) {
    goto normal_exit;
  }


  if (NULL == (ptr = index_of(name, ')'))) {
    error1("Could not find testcase name length in '%s'", name);
    retval = -1;
    goto index_failed;
  }

  *ptr = '\0';

  if (0 != testcase_append(list, name, type)) {
    error1("Unable to append '%s' to test-case list", name);
    retval = -2;
    goto testcase_append_failed;
  }

  goto normal_exit;

 testcase_append_failed:
 index_failed:
 normal_exit:
  return retval;
}

static int parse(testcase_list_t* list, const file_t* file) {
  const size_t len = file->len;

  int retval = 0;
  char* buf;
  char* p = file->buf;
  char last_c = 0;
  size_t i = 0;
  size_t idx = 0;

  int is_in_block_comment = 0;
  int is_in_row_comment = 0;
  int is_in_comment = 0;
  int is_in_body = 0;

  /*
   * This is overdoing it a bit, since we probably only need a few bytes,
   * but we never know if the buffer will be full och crap.
   */
  if (NULL == (buf = malloc(file->len + 1))) {
    error1("Out of memory while allocating buffert for testcases in '%s'", file->filename);
    retval = -1;
    goto buf_malloc_failed;
  }

  buf[0] = '\0';

  while (i < len) {
    const char c = p[i];
    const int is_block_start = (!is_in_body && !is_in_comment && ('{' == c));
    const int is_block_end = (is_in_body && !is_in_comment && ('}' == c));
    const int is_block_comment_start = (!is_in_body && (('/' == last_c) && ('*' == c)));
    const int is_block_comment_end = (!is_in_body && (('*' == last_c) && ('/' == c)));
    const int is_row_comment_start = (!is_in_body && (('/' == last_c) && ('/' == c)));
    const int is_row_comment_end = (is_in_row_comment && !is_in_body && ('\n' == c));
    const int is_line_feed = ('\n' == c);
    const int is_end_of_declaration = (!is_in_body && !is_in_comment && (is_line_feed || ('{' == c)));

    is_in_body += is_block_start;
    is_in_block_comment += is_block_comment_start;
    is_in_row_comment += is_row_comment_start;

    is_in_comment = (is_in_block_comment || is_in_row_comment);

    if (is_end_of_declaration) {
      if (0 != extract(list, buf)) {
        retval = -2;
        goto extract_failed;
      }
      idx = 0;
    }

    if (is_line_feed) {
      idx = 0;
    }
    else {
      buf[idx++] = c;
    }

    buf[idx] = '\0';

    is_in_block_comment -= is_block_comment_end;
    is_in_row_comment -= is_row_comment_end;
    is_in_body -= is_block_end;

    last_c = c;
    i++;
  }


 extract_failed:
  free(buf);
 buf_malloc_failed:
  return retval;
}

int testcase_list_init(testcase_list_t* list, const file_t* file) {
  assert((NULL != list) && "Argument 'list' must not be NULL");
  assert((NULL != file) && "Argument 'file' must not be NULL");

  return parse(list, file);
}

static void testcase_node_cleanup(testcase_node_t* node) {
  if (NULL != node->name) {
    free(node->name);
  }
  free(node);
}

void testcase_list_cleanup(testcase_list_t* list) {
  testcase_node_t* node = list->first;
  while (NULL != node) {
    testcase_node_t* next_node = node->next;
    testcase_node_cleanup(node);
    node = next_node;
  }
}
