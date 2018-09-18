#include <assert.h>
#include <string.h>

#include "error.h"
#include "debug.h"

#include "file.h"
#include "cpp.h"

static cpp_node_t* cpp_node_new(const char* buf) {
  char* s = NULL;
  cpp_node_t* node = NULL;

  if (NULL == (s = malloc(strlen(buf) + 1))) {
    error1("Out of memeory while allocating string '%s'", buf);
    goto string_malloc_failed;
  }

  if (NULL == (node = malloc(sizeof(*node)))) {
    error1("Out of memory while allocating cpp directive for '%s'", buf);
    goto node_malloc_failed;
  }
  /*
  *node = (cpp_node_t)CPP_NODE_EMPTY;
  */
  memset(node, 0, sizeof(*node));

  strcpy(s, buf);

  node->info.directive = s;

  goto normal_exit;

 node_malloc_failed:
 string_malloc_failed:
  free(s);
 normal_exit:
  return node;
}

static void cpp_list_append(cpp_list_t* list, const char* buf) {
  cpp_node_t* node = cpp_node_new(buf);

  if (NULL == node) {
    return;
  }

  if (NULL == list->first) {
    list->first = node;
  }
  if (NULL != list->last) {
    list->last->next = node;
  }
  list->last = node;
}

static int extract_cpp_directives(void* list_ptr, file_parse_state_t* state, const char c, const size_t line, const size_t col, const size_t offset) {
  cpp_list_t* list = (cpp_list_t*)list_ptr;

  if (('#' == c) && ('\n' == state->last_c)) {
    state->idx = 0;
    state->buf[state->idx] = '\0';
  }

  if ('\n' == c) {
    if ('#' == state->buf[0]) {
      if (strstr(state->buf, "#ifndef _TARSIO_DATA_")) {
	debug1("%s", state->buf);
	cpp_list_append(list, "#ifdef _TARSIO_DATA_");
      }
      else {
	debug1("%s", state->buf);
	cpp_list_append(list, state->buf);
      }
    }
    state->buf[0] = '\0';
    state->idx = 0;
  }
  else {
    state->buf[state->idx++] = c;
    state->buf[state->idx] = '\0';
  }

  /*
   * Try to only take the top part of the file. The includes are the most
   * important to catch.
   */
  if ((state->buf == strstr(state->buf, "test(")) ||
      (state->buf == strstr(state->buf, "module_test("))) {
    state->done = 1;
  }

  state->last_c = c;

  return 0;
}


int cpp_list_init(cpp_list_t* list, const file_t* file) {
  assert((NULL != list) && "Argument 'list' must not be NULL");
  assert((NULL != file) && "Argument 'file' must not be NULL");

  file_parse(extract_cpp_directives, list, file, PARSE_DECLARATIONS);

  return 0;
}

static void cpp_node_cleanup(cpp_node_t* node) {
  if (node->info.directive) {
    free(node->info.directive);
  }
  free(node);
#ifdef PARANOIA
  *node = (cpp_node_t)CPP_NODE_EMPTY;
#endif
}

void cpp_list_cleanup(cpp_list_t* list) {
  cpp_node_t* node = NULL;
  assert((NULL != list) && "Argument 'list' must not be NULL");
  node = list->first;
  while (NULL != node) {
    cpp_node_t* next_node = node->next;
    cpp_node_cleanup(node);
    node = next_node;
  }
#ifdef PARANOIA
  *list = (cpp_list_t)CPP_LIST_EMPTY;
#endif
}
