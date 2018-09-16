#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static size_t fsize(FILE *fd) {
  size_t file_size = 0;
  (void)fseek(fd, 0L, SEEK_END);
  file_size = ftell(fd);
  fseek(fd, 0L, SEEK_SET);
  return file_size;
}

struct file_s {
  size_t size;
  char* filename;
  char* buf;
};
typedef struct file_s file_t;

struct datatype_s {
  char* name;
  char* suffix; /* Usually pointer declarations with one or two '*' */
};
typedef struct datatype_s datatype_t;

struct arg_node_s {
  struct arg_node_s* next;
  char* raw;
  datatype_t datatype;
  char* name;
};
typedef struct arg_node_s arg_node_t;

struct arg_list_s {
  arg_node_t* first;
  arg_node_t* last;
};
typedef struct arg_list_s arg_list_t;

struct prototype_node_s {
  struct prototype_node_s* next;
  char* raw_decl;
  char* raw_args;
  arg_list_t arg_list;
  datatype_t* rettype;
  char* symbol;
  size_t usage;
  size_t line;
  int is_inline;
  int is_static;
  size_t offset;
};
typedef struct prototype_node_s prototype_node_t;

struct prototype_list_s {
  prototype_node_t* first;
  prototype_node_t* last;
};
typedef struct prototype_list_s prototype_list_t;

struct testcase_node_s {
  char* name;
  struct testcase_node_s* next;
};
typedef struct testcase_node_s testcase_node_t;

struct testcase_list_s {
  testcase_node_t* first;
  testcase_node_t* last;
};
typedef struct testcase_list_s testcase_list_t;

struct call_node_s {
  struct call_node_s* next;
  prototype_node_t* prototype_node;
  size_t offset;
};
typedef struct call_node_s call_node_t;

struct call_list_s {
  call_node_t *first;
  call_node_t *last;
};
typedef struct call_list_s call_list_t;

datatype_t* extract_return_type(const char* buf, const char* symbol, int* is_inline, int* is_static) {
  const char* b = &buf[0];
  char* s = strstr(buf, symbol);
  datatype_t* retval = NULL;
  size_t len = s - b;

  if ((s - b) > (long)len) {
    return NULL;
  }

  while (b < s) {
    if (b == strstr(b, "extern ")) {
      b += strlen("extern ");
      continue;
    }
    if (b == strstr(b, "static ")) {
      b += strlen("static ");
      *is_static = 1;
      continue;
    }
    if (b == strstr(b, "inline ")) {
      b += strlen("inline ");
      *is_inline = 1;
      continue;
    }
    break;
  }
  if (b > s) {
    return NULL;
  }
  len = s - b ;

  retval = malloc(sizeof(datatype_t));
  if (NULL == retval) {
    fprintf(stderr, "ERROR: Out of memory.\n");
    return NULL;
  }

  retval->name = malloc(len + 1);
  if (NULL == retval->name) {
    fprintf(stderr, "ERROR: Out of memory.\n");
    free(retval);
    return NULL;
  }

  /* Set various datatype properties */

  /* Copy the datatype name */
  memcpy(retval->name, b, len);
  retval->name[len - 1] = 0;
  retval->suffix = "";

  /* Quite ugly hack to find ptr return type */
  if (' ' == retval->name[len - 2]) {
    if ('*' == b[len - 1]) {
      retval->name[len - 2] = '*';
    }
  }

  return retval;
}

char* extract_function_name(const char* buf) {
  char* retval = NULL;
  char symbol[1024];
  const size_t len = strlen(buf);
  size_t i = 0;
  size_t idx = 0;
  for (i = 0; i < len; i++) {
    if ((' ' == buf[i]) || ('*' == buf[i])) {
      idx = 0;
      continue;
    }
    if ('(' == buf[i]) {
      break;
    }
    symbol[idx] = buf[i];
    idx++;
    symbol[idx] = 0;
  }
  retval = malloc(idx + 1);
  if (NULL == retval) {
    fprintf(stderr, "ERROR: Out of memory.\n");
    return NULL;
  }
  strcpy(retval, symbol);
  return retval;
}

void extract_prototypes(prototype_list_t* list, char c, size_t line_count, size_t offset) {
  static char buf[1024];
  static int buf_idx = 0;
  static int skip_until_line_feed = 0;
  static char last_c = 0;
  static char last_non_white_space_c = 0;
  static int paren_count = 0;
  static int found_parenthesis = 0;
  int linefeed_in_arguments = 0;
  const int is_white_space = ((' ' == c) || ('\n' == c));
  const int is_left_parentheis = ('(' == c);
  const int is_right_parentheis = (')' == c);
  const int multiple_line_feeds = (('\n' == c) && ('\n' == last_c));
  const int multiple_whitespaces = ((' ' == c) && (' ' == last_c));
  const int first_column_of_row = (('\n' == last_c) || (0 == buf_idx));
  const int pre_processor_directive = (first_column_of_row && ('#' == c));
  const int is_last_character_of_prototype = (('\n' == c) && (';' == last_c));
  const int is_last_character_of_function_head = ((('\n' == c) && (')' == last_c)) || (('\n' == c) && ' ' == last_c));
  const int is_last_character = (found_parenthesis && (is_last_character_of_prototype || is_last_character_of_function_head));
  int skip_character = 0;

  assert((1 == (1 == 1)) && "This code assumes that boolean true is equal to 1");
  assert((0 == (0 == 1)) && "This code assumes that boolean false is equal to 0");

  if (skip_until_line_feed && ('\n' == c)) {
    skip_until_line_feed = 0;
  }

  if (pre_processor_directive) {
    skip_until_line_feed = 1;
  }

  if (is_left_parentheis) {
    paren_count++;
  }

  if (is_right_parentheis) {
    paren_count--;
    if (paren_count == 0) {
      found_parenthesis = 1;
    }
  }

  linefeed_in_arguments = (('\n' == c) && (0 != paren_count));

  skip_character = (multiple_line_feeds || multiple_whitespaces || skip_until_line_feed || linefeed_in_arguments);

  if (!skip_character) {
    if (is_last_character) {

      /*
       * Here we _shuld_ have a function prototype to add to the list
       */

      if (';' != last_non_white_space_c) {
        buf[buf_idx] = ';';
        buf[buf_idx+1] = 0;
        buf_idx++;
      }

      if (NULL == strstr(buf, "typedef ")) {

      /*
       * Add new prototype node
       */
      prototype_node_t* node = malloc(sizeof(prototype_node_t));
      if (NULL == node) {
        fprintf(stderr, "ERROR: Out of memory\n");
        return;
      }
      memset(node, 0, sizeof(*node));

      node->raw_decl = malloc(strlen(buf) + 1);
      strcpy(node->raw_decl, buf);

      node->symbol = extract_function_name(buf);
      node->line = line_count; /* TODO: Probably subtract any number of linefeeds found in the declaration too */
      node->offset = offset;

      if (NULL == list->first) {
        list->first = node;
      }
      if (NULL != list->last) {
        list->last->next = node;
      }
      list->last = node;

      found_parenthesis = 0;
      buf_idx = 0;
      }
    }

    /*
     * Add characters to the temporary buffer unless there was a line-feed, by all reason it should start over
     * the buffer filling, since there should not be in-the-middle line feeds left anyomre.
     */
    if ('\n' != c) {
      buf[buf_idx] = c;
      buf[buf_idx+1] = 0;
      buf_idx++;
    }
    else {
      buf_idx = 0;
      buf[buf_idx] = 0;
    }
  }

  if (!is_white_space) {
    last_non_white_space_c = c;
  }

  last_c = c;
}

void extract_calls(call_list_t* list, prototype_list_t* prototype_list, char c, size_t offset, size_t line) {
  static char buf[1024] = "";
  static size_t idx = 0;

  if ((('a' > c) || ('z' < c)) && (('0' > c) || ('9' < c)) && ('_' != c)) {
    prototype_node_t* node = prototype_list->first;
    while (NULL != node) {
      if (0 == strcmp(buf, node->symbol)) {
        call_node_t* cnode = malloc(sizeof(call_node_t));

        fprintf(stderr, "DEBUG: Found '%s' usage on line %lu\n", node->symbol, line);

        node->usage++;
        idx = 0;

        cnode->next = 0;
        cnode->offset = offset;
        cnode->prototype_node = node;
        if (NULL == list->first) {
          list->first = cnode;
        }
        if (NULL != list->last) {
          list->last->next = cnode;
        }
        list->last = cnode;

        break;
      }
      node = node->next;
    }
    idx = 0;
    buf[idx] = 0;
  }
  else {
    buf[idx] = c;
    idx++;
    buf[idx] = 0;
  }

}

enum parse_pass_e {
  PROTOTYPES,
  CALLS,
  REPLACE,
  STRUCT,
  MOCK,
  NONE
};
typedef enum parse_pass_e parse_pass_t;

char* extract_raw_args(const char* buf, const char* symbol) {
  int paren = 0;
  size_t raw_arg_len = 0;
  const size_t len = strlen(buf);
  char* b = strstr(buf, symbol);
  char* start = NULL;
  char* retval = NULL;
  b += strlen(symbol);
  while (b < (buf + len)) {
    if (NULL != start) {
      raw_arg_len++;
    }
    if ('(' == *b) {
      if (0 == paren) {
        start = b + 1;
      }
      paren++;
    }
    else if (')' == *b) {
      paren--;
      if (0 == paren) {
        break;
      }
    }
    b++;
  }
  retval = malloc(raw_arg_len);
  if (NULL == retval) {
    fprintf(stderr, "ERROR: Out of memory\n");
    return NULL;
  }
  memcpy(retval, start, raw_arg_len - 1);
  retval[raw_arg_len - 1] = '\0';
  return retval;
}

void extract_args(arg_list_t* list, const char* buf) {
  const size_t len = strlen(buf);
  size_t l = 0;
  char* b = (char*)buf;
  char* start = NULL;
  int paren = 0;
  while (b < buf + len + 1) {
    paren += (('(' == *b) || ('[' == *b));
    paren -= ((')' == *b) || (']' == *b));
    if ((NULL == start) && (' ' != *b)) {
      start = b;
    }
    if (((',' == *b) || (b == buf + len)) && (0 == paren)) {
      l = b - start;

      if (0 != l) {
        char* c = NULL;

        arg_node_t* node = malloc(sizeof(arg_node_t));
        if (NULL == node) {
          fprintf(stderr, "ERROR: Out of memory\n");
          return;
        }
        memset(node, 0, sizeof(*node));

        node->raw = malloc(l + 1);
        memcpy(node->raw, start, l);
        node->raw[l] = 0;

        if (0 == strcmp("...", node->raw)) {
        }
        else {
          size_t datatype_name_len = strlen(node->raw);
          c = &node->raw[l - 1];
          fprintf(stderr, "DEBUG: ffooo '%s'\n", node->raw);
          /*
          while ((' ' != *c) && ('*' != *c) && (',' != *c)) {
          */
          while ((' ' != *c) && (',' != *c)) {
            c--;
            datatype_name_len--;
          }
          datatype_name_len--;
          c++;

          fprintf(stderr, "DEBUG: '%c'\n", node->raw[datatype_name_len + 1]);
          if ('*' == node->raw[datatype_name_len + 1]) {
            fprintf(stderr, "DEBUG: BAAA\n");
            datatype_name_len++;
            datatype_name_len++;
            c++;
          }

          node->datatype.name = malloc(datatype_name_len + 1);
          if (NULL == node->datatype.name) {
            fprintf(stderr, "ERROR: Out of memory\n");
            return;
          }
          memcpy(node->datatype.name, start, datatype_name_len);
          node->datatype.name[datatype_name_len] = '\0';

          node->name = malloc(strlen(c));
          if (NULL == node->name) {
            fprintf(stderr, "ERROR: Out of memory\n");
            return;
          }
          strcpy(node->name, c);
          fprintf(stderr, "DEBUG: node->datatype.name '%s' node->name '%s'\n", node->datatype.name, node->name);
        }
        if (NULL == list->first) {
          list->first = node;
        }
        if (NULL != list->last) {
          list->last->next = node;
        }
        list->last = node;
      }
      start = NULL;
      l = 0;
    }
    else {
      l++;
    }
    b++;
  }
}

void extract_prototype_details(prototype_node_t* node) {
  node->is_inline = 0;
  node->is_static = 0;
  node->rettype = extract_return_type(node->raw_decl, node->symbol, &node->is_inline, &node->is_static);
  node->raw_args = extract_raw_args(node->raw_decl, node->symbol);
  extract_args(&node->arg_list, node->raw_args);
}

void extract_all_prototype_details(prototype_list_t* list) {
  prototype_node_t* node = list->first;
  while (NULL != node) {
    if (0 != node->usage) {
      extract_prototype_details(node);
    }
    node = node->next;
  }
}

void generate_prototypes(prototype_list_t* list) {
  prototype_node_t* node = list->first;
  while (NULL != node) {
    if (0 != node->usage) {
      printf("%s%s __tarsio_proxy__%s(", node->rettype->name, node->rettype->suffix, node->symbol);
      arg_node_t* inode = node->arg_list.first;
      while (NULL != inode) {
        if (0 == strcmp(inode->raw, "...")) {
          printf("...");
        }
        else {
          printf("%s %s", inode->datatype.name, inode->name);
        }
        inode = inode->next;
        if (NULL != inode) {
          printf(", ");
        }
      }
      printf(");\n");
    }
    node = node->next;
  }
}

void generate_structs(prototype_list_t* list) {
  int argcnt = 0;
  prototype_node_t* node = list->first;
  while (NULL != node) {
    if (0 != node->usage) {
      printf("  struct { /* tarsio_generated_for_header */ \n");
      printf("    size_t call_count; /* tarsio_generated_for_header */ \n");
      if (0 != strcmp(node->rettype->name, "void")) {
        printf("    %s retval; /* tarsio_generated_for_header */ \n", node->rettype->name);
      }
      printf("    %s (*func)(", node->rettype->name);
      arg_node_t* inode = node->arg_list.first;
      while (NULL != inode) {
        if (0 == strcmp(inode->raw, "...")) {
          break;
        }
        else {
          printf("%s", inode->datatype.name);
        }
        inode = inode->next;
        if ((NULL != inode) && (0 != strcmp(inode->raw, "..."))) {
          printf(", ");
        }
      }
      printf("); /* tarsio_generated_for_header */ \n");
      printf("    struct { /* tarsio_generated_for_header */ \n");
      printf("      int __just_to_allow_no_args_at_all__; /* tarsio_generated_for_header */ \n");
      inode = node->arg_list.first;
      argcnt = 0;
      while (NULL != inode) {
        if (0 == strcmp(inode->raw, "...")) {
          printf("      /* Variadic arguments not supported */ /* tarsio_generated_for_header */ \n");
        }
        else {
          const size_t len = strlen(inode->datatype.name);
          size_t i = 0;
          size_t pi = 0;
          size_t skip = 0;
          char buf[1024];
          memset(buf, 0, sizeof(buf));
          while (i < len) {
            if (&inode->datatype.name[i] == strstr(&inode->datatype.name[i], "__restrict ")) {
              fprintf(stderr, "DEBUG: found __restrict\n");
              skip = strlen("__restrict ");
            }
            else if (&inode->datatype.name[i] == strstr(&inode->datatype.name[i], "const ")) {
              fprintf(stderr, "DEBUG: found const\n");
              skip = strlen("const ");
            }
            if (skip > 0) {
              skip--;
            }
            else {
              buf[pi] = inode->datatype.name[i];
              pi++;
            }
            i++;
          }
          fprintf(stderr, "DEBUG: '%s'\n", buf);
          printf("      %s arg%d; /* tarsio_generated_for_header */ \n", buf, argcnt);
          argcnt++;
        }
        inode = inode->next;
      }
      printf("    } args; /* tarsio_generated_for_header */ \n");
      printf("  } %s; /* tarsio_generated_for_header */ \n /* tarsio_generated_for_header */ \n", node->symbol);
    }
    node = node->next;
  }
}

void generate_mocks(prototype_list_t* list) {
  int argcnt = 0;
  prototype_node_t* node = list->first;
  while (NULL != node) {
    if (0 != node->usage) {
      printf("/* USAGE: %lu */\n", node->usage);
      printf("%s __tarsio_proxy__%s(", node->rettype->name, node->symbol);
      arg_node_t* inode = node->arg_list.first;
      while (NULL != inode) {
        if (0 == strcmp(inode->raw, "...")) {
          printf("...");
        }
        else {
          printf("%s %s", inode->datatype.name, inode->name);
        }
        inode = inode->next;
        if (NULL != inode) {
          printf(", ");
        }
      }
      printf(") {\n");
      printf("  fprintf(stderr, \"DEBUG: calling '%s'\\n\");", node->symbol);
      printf("  tarsio_mock_ptr->%s.call_count++;\n", node->symbol);
      inode = node->arg_list.first;
      argcnt = 0;
      while (NULL != inode) {
        if (0 == strcmp(inode->raw, "...")) {
        }
        else {
          int constified = 0;
          const size_t len = strlen(inode->datatype.name);
          size_t i = 0;
          size_t pi = 0;
          size_t skip = 0;
          char buf[1024];
          memset(buf, 0, sizeof(buf));
          while (i < len) {
            if (&inode->datatype.name[i] == strstr(&inode->datatype.name[i], "__restrict ")) {
              fprintf(stderr, "DEBUG: found __restrict\n");
              skip = strlen("__restrict ");
            }
            else if (&inode->datatype.name[i] == strstr(&inode->datatype.name[i], "const ")) {
              fprintf(stderr, "DEBUG: found const\n");
              skip = strlen("const ");
              constified = 1;
            }
            if (skip > 0) {
              skip--;
            }
            else {
              buf[pi] = inode->datatype.name[i];
              pi++;
            }
            i++;
          }
          /*
          printf("  tarsio_mock.%s.args.%s = %s;\n", node->symbol, inode->name, inode->name);
          */
          if (0 == constified) {
            printf("  tarsio_mock_ptr->%s.args.arg%d = %s;\n", node->symbol, argcnt, inode->name);
          }
          else {
            printf("  tarsio_mock_ptr->%s.args.arg%d = (%s)%s;\n", node->symbol, argcnt, buf, inode->name);
          }
          argcnt++;
        }
        inode = inode->next;
      }
      printf("  fprintf(stderr, \"Args handled\\n\");");
      printf("  fprintf(stderr, \"DEBUG: %%p\\n\", (void*)tarsio_mock_ptr);");
      printf("  if (0 == tarsio_mock_ptr->%s.func) {\n", node->symbol);
      printf("    fprintf(stderr, \"Returning mock return value\\n\");\n");
      printf("    return");
      if (0 != strcmp(node->rettype->name, "void")) {
        printf(" tarsio_mock_ptr->%s.retval", node->symbol);
      }
      printf(";\n");
      printf("  }\n");
      printf("  ");
      if (0 != strcmp(node->rettype->name, "void")) {
        printf("return ");
      }
      printf("tarsio_mock_ptr->%s.func(", node->symbol);
      inode = node->arg_list.first;
      while (NULL != inode) {
        if (0 == strcmp(inode->raw, "...")) {
          break;
        }
        printf("%s", inode->name);
        inode = inode->next;
        if ((NULL != inode) && (0 != strcmp(inode->raw, "..."))) {
          printf(", ");
        }
      }
      printf(");\n");
      printf("}\n\n");
    }
    node = node->next;
  }
}

void replace_calls(call_list_t* list, char c, prototype_list_t* prototype_list, const size_t line, const size_t first_used_function_line) {
  static size_t offs = 0;
  static int printed_prototypes = 0;
  call_node_t* node = list->first;

  if ((0 == printed_prototypes) && (0 != first_used_function_line) && (line == first_used_function_line)) {
    generate_prototypes(prototype_list);
    printed_prototypes = 1;
  }

  while (NULL != node) {
    if ((node->offset - strlen(node->prototype_node->symbol)) == offs) {
      printf("__tarsio_proxy__");
    }
    node = node->next;
  }
  putc(c, stdout);
  offs++;
}

int main(int argc, char* argv[]) {
  size_t i = 0;
  FILE* fd = NULL;
  file_t pre_processed_source = {0, NULL, NULL};
  char* test_cases_source_filename = NULL;
  int num_paren = 0;
  int num_braces = 0;
  int in_function_body = 0;
  prototype_list_t prototype_list = {NULL, NULL};
  call_list_t call_list = {NULL, NULL};
  testcase_list_t testcase_list = {NULL, NULL};
  parse_pass_t parse_pass;
  size_t read_size = 0;

  if (argc != 3) {
    fprintf(stderr, "ERROR: Wrong number of arguments\n");
    exit(1);
  }

  pre_processed_source.filename = argv[1];
  test_cases_source_filename = argv[2];

  fd = fopen(pre_processed_source.filename, "r");
  if (NULL == fd) {
    fprintf(stderr, "ERROR: Unable to open file '%s' for reading.\n",
            pre_processed_source.filename);
    exit(1); /* TODO: Use correct exit code for file not found */
  }

  pre_processed_source.size = fsize(fd);
  if (0 == pre_processed_source.size) {
    fprintf(stderr, "ERROR: Pre-processed file '%s' is zero bytes.\n", pre_processed_source.filename);
    fclose(fd);
    exit(2); /* TODO: Use correct exit code for I/O error */
  }

  pre_processed_source.buf = malloc(pre_processed_source.size + 1);
  if (NULL == pre_processed_source.buf) {
    fprintf(stderr, "ERROR: Out of memory when allocating memory for pre-processed source file\n");
    fclose(fd);
    exit(3); /* TODO: Use correct exit code for out of memory */
  }

  if (pre_processed_source.size != (read_size = fread(pre_processed_source.buf, 1, pre_processed_source.size, fd))) {
    fprintf(stderr, "ERROR: Read error (exptected %lu bytes but read %lu)\n", pre_processed_source.size, read_size);
    free(pre_processed_source.buf);
    fclose(fd);
    exit(4); /* TODO: Use correct exit code for I/O error */
  }
  pre_processed_source.buf[pre_processed_source.size] = 0;

  fclose(fd);

  parse_pass = PROTOTYPES;

  size_t first_used_function_line = 0;

  while (NONE != parse_pass) {
    size_t line_count = 0;
    for (i = 0; (i < pre_processed_source.size) && ((PROTOTYPES == parse_pass) || (CALLS == parse_pass) || (REPLACE == parse_pass)); i++) {
      const char c = pre_processed_source.buf[i];
      const int is_code_block_start = ('{' == c) ? 1 : 0;
      const int is_code_block_end = ('}' == c) ? 1 : 0;
      const int is_left_parentheis = ('(' == c) ? 1 : 0;
      const int is_right_parentheis = (')' == c) ? 1: 0;
      const int no_parenthesis_count = (num_paren == 0);
      const int no_braces_count = (num_braces == 0);
      const int one_braces_count = (num_braces == 1);
      const int is_entering_function_body = ((is_code_block_start) && (no_parenthesis_count) && (no_braces_count));
      const int is_exiting_function_body = ((is_code_block_end) && (no_parenthesis_count) && (one_braces_count));
      num_braces += is_code_block_start;
      in_function_body += is_entering_function_body;

      switch (parse_pass) {
      case PROTOTYPES:
        if (0 == in_function_body) {
          extract_prototypes(&prototype_list, c, line_count, i);
        }
        break;
      case CALLS:
        if (1 == in_function_body) {
          extract_calls(&call_list, &prototype_list, c, i, line_count);
        }
        break;
      case REPLACE:
        replace_calls(&call_list, c, &prototype_list, line_count, first_used_function_line);
        break;
      case STRUCT:
        fprintf(stderr, "ERROR: Logic error in parser\n");
        break;
      case MOCK:
        fprintf(stderr, "ERROR: Logic error in parser\n");
        break;
      case NONE:
        fprintf(stderr, "ERROR: Logic error in parser\n");
        break;
      }

      line_count += ('\n' == c);

      num_braces -= is_code_block_end;
      in_function_body -= is_exiting_function_body;
      num_paren += is_left_parentheis;
      num_paren -= is_right_parentheis;
    }

    switch (parse_pass) {
    case PROTOTYPES:
      parse_pass = CALLS;
      break;
    case CALLS:
      parse_pass = REPLACE;
      extract_all_prototype_details(&prototype_list);
      {
        prototype_node_t* node = prototype_list.first;
        while (NULL != node) {
          if ((0 < node->usage) && ((0 == first_used_function_line) || (first_used_function_line > node->line))) {
            first_used_function_line = node->line - 1;
          }
          node = node->next;
        }
      }
      break;
    case REPLACE:
      parse_pass = STRUCT;
      break;
    case STRUCT:
      parse_pass = MOCK;
      printf("struct __tarsio_mock_data_s { /* tarsio_generated_for_header */ \n /* tarsio_generated_for_header */ \n");
      printf("  int __just_to_allow_no_mocks_at_all__;\n\n");
      generate_structs(&prototype_list);
      printf("}; /* tarsio_generated_for_header */ \n /* tarsio_generated_for_header */ \n");
      printf("typedef struct __tarsio_mock_data_s __tarsio_mock_data_t; /* tarsio_generated_for_header */\n");
      printf("static struct __tarsio_mock_data_s *tarsio_mock_ptr;\n");
      /*
      printf("extern __tarsio_mock_data_t tarsio_mock;\n");
      */
      break;
    case MOCK:
      parse_pass = NONE;
      generate_mocks(&prototype_list);
      break;
    case NONE:
      fprintf(stderr, "ERROR: Logic error in parser\n");
      break;
    }
  }

  fd = fopen(test_cases_source_filename, "r");
  if (NULL == fd) {
    fprintf(stderr, "ERROR: Unable to open file '%s' for reading.\n",
            pre_processed_source.filename);
    exit(1);
  }
  char buf[1024];
  while (NULL != fgets(buf, sizeof(buf), fd)) {
    /*
    printf("%s", buf);
    */
    if (buf == strstr(buf, "void __tarsio_test_")) {
      const size_t prefixlen = strlen("void __tarsio_test") + 1;
      const char* parenpos = strstr(buf, "(");
      const size_t namelen = parenpos - buf - prefixlen;
      testcase_node_t* node = malloc(sizeof(testcase_node_t));
      if (NULL == node) {
        fprintf(stderr, "ERROR: Out of memory\n");
        exit(1);
      }
      node->name = malloc(namelen + 1);
      if (NULL == node->name) {
        fprintf(stderr, "ERROR: Out of memory\n");
        exit(1);
      }
      memcpy(node->name, &buf[prefixlen], namelen);
      node->name[namelen + 1] = '\0';
      node->next = NULL;
      if (NULL == testcase_list.first) {
        testcase_list.first = node;
      }
      if (NULL != testcase_list.last) {
        testcase_list.last->next = node;
      }
      testcase_list.last = node;
      printf("extern void __tarsio_test_%s();\n", node->name);
    }
  }
  fclose(fd);

  free(pre_processed_source.buf);

  printf("\n"
         "void __tarsio_zero(__tarsio_mock_data_t* tarsio_mock) {\n"
         "  char* b = (char*)tarsio_mock;\n"
         "  size_t i = 0;\n"
         "  for (i = 0; i < sizeof(*tarsio_mock); i++) {\n"
         "    b[i] = 0;\n"
         "  }\n"
         "}\n\n"
         "int main(int argc, char* argv[]) {\n"
         "  __tarsio_mock_data_t tarsio_mock;\n");

  testcase_node_t* node = testcase_list.first;
  while (NULL != node) {
    printf("  __tarsio_zero(&tarsio_mock);\n");
    printf("  __tarsio_test_%s(&tarsio_mock);\n", node->name);
    node = node->next;
  }

  /*
  for (node = testcase_list.first; NULL != node; node = node->next) {
    fprintf(stderr, "%s\n", node->name);
  }
  */
  printf("}\n");

  return 0;
}
