#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "debug.h"
#include "file.h"

static size_t fsize(FILE *fd) {
  size_t file_size;
  (void)fseek(fd, 0L, SEEK_END);
  file_size = ftell(fd);
  fseek(fd, 0L, SEEK_SET);
  return file_size;
}

int file_parse(file_parse_cb_t func, void* list_ptr, const file_t* file, parse_part_t parse_part) {

  const int parse_function_bodies = ((PARSE_FUNCTION_BODIES == parse_part) || (PARSE_ALL == parse_part));
  const int parse_declarations = ((PARSE_DECLARATIONS == parse_part) || (PARSE_ALL == parse_part));

  size_t i;
  size_t line_count = 0;
  size_t column_count = 0;
  int num_braces = 0;
  int num_paren = 0;
  int in_function_body = 0;
  int in_string = 0;
  int in_comment = 0;
  file_parse_state_t state = EMPTY_FILE_PARSE_STATE;
  char last_c = 0;

  int last_do_parse = -1;
  int last_in_string = -1;
  int last_in_comment = -1;
  int last_in_function_body = -1;
  int last_is_code_block_start = -1;

  assert((0 == ('0' == ' ')) && "This code assumes that bool false is 0 for mathematical operations");
  assert((1 == ('0' == '0')) && "This code assumes that bool true is 1 for mathematical operations");

  state.buf = malloc(file->len + 1);

  for (i = 0; i < file->len; i++) {
    const char c = file->buf[i];
    const int is_comment_start = (!in_string && !in_comment && (('*' == c) && ('/' == last_c)));
    const int is_comment_end = (in_comment && (('/' == c) && ('*' == last_c)));
    const int is_string_start = (!in_comment && !in_string && (('"' == c) || ('\'' == c)));
    const int is_string_end = (in_string && ((('"' == c) || ('\'' == c)) && '\\' != last_c));
    const int is_code_block_start = (!in_comment && !in_string && ('{' == c));
    const int is_code_block_end = (!in_comment && !in_string && ('}' == c));
    const int is_left_parentheis = (!in_comment && !in_string && ('(' == c));
    const int is_right_parentheis = (!in_comment && !in_string && (')' == c));
    const int no_parenthesis_count = (num_paren == 0);
    const int no_braces_count = (num_braces == 0);
    const int one_braces_count = (num_braces == 1);
    const int is_entering_function_body = ((is_code_block_start) && (no_parenthesis_count) && (no_braces_count));
    const int is_exiting_function_body = ((is_code_block_end) && (no_parenthesis_count) && (one_braces_count));
    int do_parse;

    if (1 == state.done) {
      break;
    }

    num_braces += is_code_block_start;

    in_string += is_string_start;
    in_string -= is_string_end;

    in_comment += is_comment_start;
    in_comment -= is_comment_end;

    in_function_body += is_entering_function_body;
    in_function_body -= is_exiting_function_body;

    do_parse = (!in_comment && ((parse_function_bodies && in_function_body) ||
				(parse_declarations && !in_function_body) ||
				is_code_block_start));

    /*
    if ((last_do_parse != do_parse) ||
        (last_in_string != in_string) ||
        (last_in_comment != in_comment) ||
        (last_in_function_body != in_function_body) ||
        (last_is_code_block_start != is_code_block_start)) {
      debug5("do_parse? %d is string? %d in comment? %d in function body? %d is code block start %d", do_parse, in_string, in_comment, in_function_body, is_code_block_start);
    }
    */
    if (do_parse) {
      func(list_ptr, &state, c, line_count, column_count, i);
    }

    line_count += ('\n' == c);
    column_count -= (('\n' != c) ? -1 : column_count);

    num_braces -= is_code_block_end;
    num_paren += is_left_parentheis;
    num_paren -= is_right_parentheis;
    last_c = c;
    last_do_parse = do_parse;
    last_in_string = in_string;
    last_in_comment = in_comment;
    last_in_function_body = in_function_body;
    last_is_code_block_start = is_code_block_start;
  }

  free(state.buf);

  return 0;
}

int file_init(file_t* file, const char* filename) {
  int retval = 0;
  FILE* fd;

  assert((NULL != file) && "Argument 'file' must not be NULL");
  assert((NULL != filename) && "Argument 'filename' must not be NULL");

  if (NULL == (fd = fopen(filename, "r"))) {
    fprintf(stderr, "ERROR: File '%s' not found.\n", filename);
    retval = -1;
    goto fopen_failed;
  }

  if (0 == (file->len = fsize(fd))) {
    fprintf(stderr, "ERROR: Invalid file size of '%s'.\n", filename);
    retval = -2;
    goto fsize_failed;
  }

  if (NULL == (file->buf = malloc(file->len + 1))) {
    fprintf(stderr, "ERROR: Out of memory while reading '%s'.\n", filename);
    retval = -3;
    goto malloc_failed;
  }

  if (file->len != fread(file->buf, 1, file->len, fd)) {
    fprintf(stderr, "ERROR: Invalid read size while reading '%s'.\n", filename);
    retval = -4;
    goto fread_failed;
  }

  file->filename = (char*)filename;

  goto normal_exit;

 fread_failed:
  free(file->buf);
 malloc_failed:
 normal_exit:
 fsize_failed:
  fclose(fd);
 fopen_failed:
  return retval;
}

void file_cleanup(file_t* file) {
  assert((NULL != file) && "Argument 'file' must not be NULL");
  if (NULL != file->buf) {
    free(file->buf);
  }
}
