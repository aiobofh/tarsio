#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "warning.h"
#include "error.h"
#include "debug.h"
#include "prototype.h"

static char* extract_symbol(const char* raw) {
  char* symbol;
  size_t len = 0;
  char* p = strstr(raw, "(");
  if (NULL == p) {
    error1("Could not find first parenthesis in the detected funciton delcaration '%s'", raw);
    return NULL;
  }
  /*
   * Skip the parenthesis
   */
  p--;
  /*
   * Skip eventual white-spaces after the symbol name.
   */
  while (((' ' == *p) || ('\t' == *p)) && (p > raw)) {
    p--;
  }
  while ((p > raw) && ((' ' != *p) && ('*' != *p))) {
    len++;
    p--;
  }
  p++;
  symbol = malloc(len + 2);
  if (NULL == symbol) {
    error1("Out of memory while allocating symbol name for '%s'", raw);
    return NULL;
  }

  memcpy(symbol, p, len);
  symbol[len] = '\0';

  debug2("Extracted symbol '%s' '%s'", symbol, raw);

  return symbol;
}

static int prototype_list_find_symbol(const prototype_list_t* list, const char* symbol) {
  prototype_node_t* node;
  for (node = list->first; NULL != node; node = node->next) {
    if (0 == strcmp(symbol, node->info.symbol)) {
      return 1;
    }
  }
  return 0;
}

static prototype_node_t* prototype_node_new(const char* raw, const prototype_list_t* list) {
  prototype_node_t* node = NULL;
  char* symbol;

  symbol = extract_symbol(raw);
  if (NULL == symbol) {
    warning1("Wrongfully detected '%s' as a function declaration, but no symbol could be found", raw);
    goto extract_symbol_failed;
  }

  if (0 == strlen(symbol)) {
    warning1("Something fishy is going on, detected empty string '%s' as symbol, could be programming error in Tarsio comonents", symbol);
    goto strlen_zero;
  }

  if (1 == prototype_list_find_symbol(list, symbol)) {
    goto found_duplicate;
  }

  node = malloc(sizeof(*node));
  if (NULL == node) {
    error1("Out of memory while allocating prototype node for '%s'", raw);
    goto node_malloc_failed;
  }

  memset(node, 0, sizeof(*node));

  node->info.raw_prototype.decl = malloc(strlen(raw) + 1);
  if (NULL == node->info.raw_prototype.decl) {
    error1("Out of memory while allocating raw prototype for '%s'", raw);
    goto raw_prototype_malloc_failed;
  }

  strcpy(node->info.raw_prototype.decl, raw);
  node->info.symbol = symbol;

  goto normal_exit;

 raw_prototype_malloc_failed:
  free(node);
  node = NULL;
 node_malloc_failed:
 strlen_zero:
 found_duplicate:
  free(symbol);
 extract_symbol_failed:
 normal_exit:
  return node;
}

static void prototype_list_append(prototype_list_t* list, const char* raw, int is_function_implementation) {
  prototype_node_t* node = prototype_node_new(raw, list);
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
  node->info.is_function_implementation = is_function_implementation;
  list->cnt++;
}

static int extract_prototypes(void* list_ptr, file_parse_state_t* state, const char c, const size_t line, const size_t col, const size_t offset) {
  prototype_list_t* list = (prototype_list_t*)list_ptr;
  const char last_c = state->last_c;
  const int is_white_space = ((' ' == c) || ('\n' == c));
  const int last_is_white_space = ((' ' == last_c) || ('\n' == last_c));
  const int is_line_feed = ('\n' == c);
  const int last_is_line_feed = ('\n' == last_c);
  const int is_left_parenthesis = ('(' == c);
  const int is_right_parenthesis = (')' == c);
  const int multiple_line_feeds = (is_line_feed && last_is_line_feed);
  const int multiple_whitespaces = (is_white_space && last_is_white_space);
  const int is_last_character_of_prototype = ((is_line_feed && (';' == last_c)) || ('{' == c));
  const int is_last_character = (state->found_parenthesis && is_last_character_of_prototype);
  int skip_character;
  int linefeed_in_argument_list;
  (void)col;
  (void)offset;
  state->line_feeds_in_declaration += ('\n' == c);

  state->paren_count += is_left_parenthesis;
  if (is_right_parenthesis && (--state->paren_count == 0)) {
    state->found_parenthesis = 1;
  }

  linefeed_in_argument_list = (('\n' == c) && (0 != state->paren_count));

  skip_character = (multiple_line_feeds ||
                    multiple_whitespaces ||
                    state->skip_until_linefeed ||
                    linefeed_in_argument_list);

  if (0 == skip_character) {
    /*
     * Append the character to the buffer.
     */
    if (!is_line_feed) {
      state->buf[state->idx++] = c;
      state->buf[state->idx] = 0;
    }
  }

  /*
   * A bit tweaky.... Do only reset the buffer if any number of variables indicate we're not still in a function prototype.
   */
  if ((0 == state->paren_count) &&
      is_line_feed &&
      !is_last_character &&
      (0 == state->found_parenthesis) &&
      (0 != state->buf[0]))
    {
      state->idx = 0;
      state->buf[0] = 0;
    }

  if (1 == is_last_character) {
    int is_function_implementation = 0;
    if ('{' == c) {
      is_function_implementation = line - state->line_feeds_in_declaration;
      state->buf[state->idx - 1] = ';';
    }
    debug2("is_function_implementation %d %c", is_function_implementation, state->buf[state->idx - 1]);
    /*
     * A new prototype is found and should be added to the list of prototypes.
     */
    if (state->buf != strstr(state->buf, "typedef ")) {
      prototype_list_append(list, state->buf, is_function_implementation);
    }
    state->found_parenthesis = 0;
    state->idx = 0;
    state->buf[0] = 0;
    state->line_feeds_in_declaration = 0;
  }

  state->last_c = c;

  return 0;
}

size_t prototype_get_first_function_implementation_line(prototype_list_t* list) {
  prototype_node_t* node;
  for (node = list->first; NULL != node; node = node->next) {
    if (node->info.is_function_implementation) {
      return (size_t)node->info.is_function_implementation;
    }
  }
  return 0;
}

static void raw_prototype_cleanup(raw_prototype_t* raw_prototype) {
  free(raw_prototype->decl);
  free(raw_prototype->args);
}

static void prototype_node_cleanup(prototype_node_t* node) {
  raw_prototype_cleanup(&node->info.raw_prototype);
  datatype_cleanup(&node->info.datatype);
  free(node->info.symbol);
  argument_list_cleanup(&node->info.argument_list);
  symbol_usage_list_cleanup(&node->info.symbol_usage_list);
  free(node);
}

int prototype_list_init(prototype_list_t* list, const file_t* file) {
  assert((NULL != list) && "Argument 'list' must not be NULL");
  assert((NULL != file) && "Argument 'file' must not be NULL");

  file_parse(extract_prototypes, list, file, PARSE_DECLARATIONS);

  return 0;
}

static int find_symbol_usage(void* list_ptr, file_parse_state_t* state, const char c, const size_t line, const size_t col, const size_t offset) {
  prototype_list_t* list = (prototype_list_t*)list_ptr;
  prototype_node_t* node;

  if ((('a' > c) || ('z' < c)) && (('0' > c) || ('9' < c)) && ('_' != c)) {
    for (node = list->first; NULL != node; node = node->next) {
      if (0 == strcmp(state->buf, node->info.symbol)) {
        debug1("Searching for '%s'", node->info.symbol);
        symbol_usage_append(&node->info.symbol_usage_list, line, col, offset);
        break;
      }
    }
    state->idx = 0;
    state->buf[state->idx] = 0;
  }
  else {
    state->buf[state->idx++] = c;
    state->buf[state->idx] = 0;
  }

  return 0;
}

int prototype_usage(prototype_list_t* list, const file_t* file) {
  assert((NULL != list) && "Argument 'list' must not be NULL");
  assert((NULL != file) && "Argument 'file' must not be NULL");

  file_parse(find_symbol_usage, list, file, PARSE_FUNCTION_BODIES);

  return 0;
}

int prototype_remove_unused(prototype_list_t* list) {
  prototype_node_t* prev_node = NULL;
  prototype_node_t* node = list->first;

  while (NULL != node) {
    prototype_node_t* next_node = node->next;
    if (NULL != node->info.symbol_usage_list.first) {
      prev_node = node;
      node = next_node;
      continue;
    }

    if (list->first == node) {
      list->first = next_node;
    }
    else {
      if (NULL != prev_node) {
        prev_node->next = next_node;
      }
    }
    if (list->last == node) {
      list->last = prev_node;
    }

    debug1("Removing '%s'", node->info.symbol);

    prototype_node_cleanup(node);

    node = next_node;
    list->cnt--;
  }

  return 0;
}

static int extract_return_type(prototype_node_t* node) {
  const char* symbol = node->info.symbol;
  char* raw = node->info.raw_prototype.decl;
  const char* symbol_start = strstr(raw, symbol);
  size_t len;
  char* buf;
  size_t asterisks = 0;

  if (NULL == symbol_start) {
    error2("Could not find expected symbol '%s' in '%s'", symbol, raw);
    return -1;
  }

  /*
   * Detect linkage definitions for the function prototype.
   */
  while (raw < symbol_start) {
    if (raw == strstr(raw, "extern ")) {
      raw += strlen("extern ");
      node->info.linkage_definition.is_extern = 1;
      continue;
    }
    if (raw == strstr(raw, "static ")) {
      raw += strlen("static ");
      node->info.linkage_definition.is_static = 1;
      continue;
    }
    if (raw == strstr(raw, "inline ")) {
      raw += strlen("inline ");
      node->info.linkage_definition.is_inline = 1;
      continue;
    }
    break;
  }

  len = symbol_start - raw - 1;

  /*
   * Count asterisks
   */
  while (len > 0) {
    char c = raw[len];
    if (' ' == c) {
      /* Nop - just ignore spaces */
    }
    else if ('*' == c) {
      asterisks++;
    }
    else {
      break;
    }
    len--;
  }
  len++;
  node->info.datatype.datatype_definition.is_pointer = asterisks;

  /*
   * Copy the datatype name to the prototype node.
   */
  buf = malloc(len + 1);
  if (NULL == buf) {
    error1("Out of memory while allocating return type for '%s'", symbol);
    return -2;
  }

  memcpy(buf, raw, len);
  buf[len] = '\0';

  node->info.datatype.name = buf;

  return 0;
}

int prototype_extract_return_types(prototype_list_t* list) {
  prototype_node_t* node;
  for (node = list->first; NULL != node; node = node->next) {
    if (0 != extract_return_type(node)) {
      error1("Return type could not be extracted for '%s'", node->info.symbol);
      return -1;
    }
  }
  return 0;
}

static int extract_arguments(prototype_node_t* node) {
  const char* symbol = node->info.symbol;
  char* raw = node->info.raw_prototype.decl;
  const char* symbol_start = strstr(raw, symbol);
  int paren_count;
  char* start_args;
  size_t len;
  int astrisks = 0;
  int dummy_cnt = 0;
  char* last_start;
  char* last_space;

  debug1("Extracting arguments for '%s'", node->info.symbol);

  raw = (char*)symbol_start + strlen(symbol);

  /*
   * Skip whitespaces between symbol name and first parenthesis.
   */
  while ('(' != *raw) {
    raw++;
  }
  raw++;

  paren_count = 1;
  start_args = raw;
  last_start = start_args;
  last_space = last_start;
  while (0 < paren_count) {
    char c = *raw;
    paren_count += ('(' == c);
    paren_count -= (')' == c);
    if ((1 == paren_count) && ((' ' == c) || ('*' == c))) {
      last_space = raw + 1;
    }
    if ((1 == paren_count) && ('*' == c)) {
      astrisks++;
    }
    if (((1 == paren_count) && (',' == c)) || (0 == paren_count)) {
      int is_variadic = 0;
      int is_const = 0;
      char* end = raw;
      char* arg_name;
      char* type_name;
      raw += (',' == c);
      while (' ' == *raw) {
        raw++;
      }
      if (last_start == strstr(last_start, "const ")) {
        is_const = 1;
        last_start += strlen("const ");
      }

      /*
       * Argument name length
       */
      len = end - last_space;
      arg_name = malloc(len + 1);
      if (NULL == arg_name) {
        error1("Out of memory while allocating argument name for '%s'", symbol);
        return -1;
      }
      memcpy(arg_name, last_space, len);
      arg_name[len] = '\0';

      debug1(" Located arg name '%s'", arg_name);

      if (0 == strcmp("...", arg_name)) {
        is_variadic = 1;
        type_name = arg_name;
        arg_name = NULL;
      }
      else {

        /*
         * Datatype name length
         */
        debug1(" Eliminating other compiler specific keywords in '%s'", last_start);
        if (strstr(last_start, "*__restrict")) {
          last_space -= strlen("*__restrict");
        }

        last_space--;
        while ((' ' == *last_space) || ('*' == *last_space) || (')' == *last_space)) {
          last_space--;
        }
        len = last_space - last_start + 1;

        debug1(" Type name len %lu", len);

        if (0 > last_space - last_start + 1) {
          debug0(" Probably the last (anonymous) argument...");
          len = 0;
        }

        type_name = malloc(len + 1);
        if (NULL == type_name) {
          error2("Out of memory while allocating data type name for argument '%s' in '%s'", arg_name, symbol);
          return -2;
        }

        if (len > 0) {
          memcpy(type_name, last_start, len);
        }
        type_name[len] = '\0';

        debug1(" Located type-name '%s'", type_name);

        /*
         * Name-less agrument. TODO: Should this be??
         */
        if (0 == strcmp("", type_name)) {
          free(type_name);
          type_name = arg_name;
          arg_name = malloc(strlen("dummyXX") + 1 + 8);
          sprintf(arg_name, "dummy%02d", dummy_cnt);
          dummy_cnt++;
        }

        /*
         * Name-less agrument.
         */
        if (0 == strcmp("", arg_name)) {
          free(arg_name);
          arg_name = malloc(strlen("dummyXX") + 1 + 8);
          sprintf(arg_name, "dummy%02d", dummy_cnt);
          dummy_cnt++;
        }
      }

      if ((0 == astrisks) && (0 == strcmp("void", type_name))) {
        free(type_name);
        free(arg_name);
      }
      else {
        debug2(" Appending '%s' '%s' to list", type_name, arg_name);
        if (0 != argument_list_append(&node->info.argument_list, type_name, arg_name, is_const, is_variadic, astrisks)) {
          error2("Could not append argument '%s' to '%s'", arg_name, symbol);
          return -3;
        }
        debug0(" All good");
      }

      last_start = raw;
      last_space = raw;
      astrisks = 0;
    }
    raw++;
  }

  return 0;
}

int prototype_extract_arguments(prototype_list_t* list) {
  prototype_node_t* node;
  for (node = list->first; NULL != node; node = node->next) {
    if (0 != extract_arguments(node)) {
      error1("Return type could not be extracted for '%s'", node->info.symbol);
      return -1;
    }
  }
  return 0;
}

void prototype_list_cleanup(prototype_list_t* list) {
  prototype_node_t* node;
  assert((NULL != list) && "Argument 'list' must not be NULL");
  node = list->first;
  while (NULL != node) {
    prototype_node_t* next_node = node->next;
    prototype_node_cleanup(node);
    node = next_node;
  }
}

/*
 * TODO: Refactor generate_prototype to write to disk more effeicient
 */
void generate_prototype(prototype_node_t* node, const char* prefix, const char* prepend, const char* suffix) {
  argument_node_t* anode;
  int i;
  if (NULL != prefix) {
    printf("%s", prefix);
  }
  printf("%s", node->info.datatype.name);
  for (i = 0; i < node->info.datatype.datatype_definition.is_pointer; i++) {
    printf("*");
  }
  printf(" ");
  if (NULL != prepend) {
    printf("%s", prepend);
  }
  printf("%s(", node->info.symbol);
  for (anode = node->info.argument_list.first; NULL != anode; anode = anode->next) {
    if (anode->info.datatype.datatype_definition.is_variadic) {
      printf("...");
      break;
    }
    if (anode->info.datatype.datatype_definition.is_const) {
      printf("const ");
    }
    printf("%s", anode->info.datatype.name);
    for (i = 0; i < anode->info.datatype.datatype_definition.is_pointer; i++) {
      printf("*");
    }
    if (anode->info.name) {
      printf(" %s", anode->info.name);
    }
    if (NULL != anode->next) {
      printf(", ");
    }
  }
  printf(")%s\n", suffix);
}
