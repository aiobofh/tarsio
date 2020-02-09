#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "debug.h"
#include "file.h"

static size_t fsize(FILE *fd) {
  size_t file_size;
  (void)fseek(fd, 0L, SEEK_END);
  file_size = ftell(fd);
  rewind(fd);
  return file_size;
}

int file_parse(file_parse_cb_t func, void* list_ptr, const file_t* file, parse_part_t parse_part, int skip_strings) {

  const int parse_function_bodies = ((PARSE_FUNCTION_BODIES == parse_part) || (PARSE_ALL == parse_part));
  const int parse_declarations = ((PARSE_DECLARATIONS == parse_part) || (PARSE_ALL == parse_part));

  size_t i;
  size_t line_count = 1;
  size_t column_count = 0;
  int num_braces = 0;
  int num_paren = 0;
  int in_function_body = 0;
  int in_single_quote_string = 0;
  int in_double_quote_string = 0;
  int last_in_string = 0;
  int in_string = 0;
  int in_comment = 0;
  file_parse_state_t state = EMPTY_FILE_PARSE_STATE;
  char last_c = 0;
  char last_last_c = 0;
  size_t last_linefeed_offset = 0;
  size_t last_function_start = 0;

  assert((0 == ('0' == ' ')) && "This code assumes that bool false is 0 for mathematical operations");
  assert((1 == ('0' == '0')) && "This code assumes that bool true is 1 for mathematical operations");

  state.buf = malloc(file->len + 1);

  for (i = 0; i < file->len; i++) {
    char c = file->buf[i];
    /*
     * TODO: Make the file-parser handle C++ style line-comments.
     */
    const int is_comment_start = (!in_string && !in_comment && (('*' == c) && ('/' == last_c)));
    const int is_comment_end = (in_comment && (('/' == c) && ('*' == last_c)));
    const int is_escaped_single_quote = (in_single_quote_string && ('\'' == c) && ('\\' == last_c) && ('\\' != last_last_c));
    const int is_escaped_double_quote = (in_double_quote_string && ('"' == c) && ('\\' == last_c) && ('\\' != last_last_c));
    const int is_double_quote_string_start = (!in_comment && !in_string && (('"' == c)));
    const int is_single_quote_string_start = (!in_comment && !in_string && (('\'' == c)));
    const int is_string_start = is_single_quote_string_start || is_double_quote_string_start;
    const int is_double_quote_string_end = (in_double_quote_string && ('"' == c) && (!is_escaped_double_quote));
    const int is_single_quote_string_end = (in_single_quote_string && ('\'' == c) && (!is_escaped_single_quote));
    const int is_string_end = is_single_quote_string_end || is_double_quote_string_end;
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
    int is_inserted_space = 0;

    if (1 == state.done) {
      break;
    }

    num_braces += is_code_block_start;

    in_single_quote_string += is_single_quote_string_start;
    in_single_quote_string -= is_single_quote_string_end;

    in_double_quote_string += is_double_quote_string_start;
    in_double_quote_string -= is_double_quote_string_end;

    in_string += is_string_start;
    in_string -= is_string_end;

    in_comment += is_comment_start;
    in_comment -= is_comment_end;

    if (is_entering_function_body) {
      last_function_start = last_linefeed_offset;
    }

    in_function_body += is_entering_function_body;
    in_function_body -= is_exiting_function_body;

    if (!in_string &&
        !in_comment &&
        !in_function_body &&
        !no_parenthesis_count &&
        no_braces_count &&
        '\n' == c) {
      is_inserted_space = 1;
      if ('\n' == c) {
        line_count += 1;
        column_count = 1;
      }
      c = ' ';
    }

    do_parse = (!in_comment && ((parse_function_bodies && in_function_body) ||
				(parse_declarations && !in_function_body) ||
				is_code_block_start || is_inserted_space));

    if (do_parse) {
      if ((in_string || last_in_string) && skip_strings) {
        /* Do not care about strings kontents */
      }
      else if ('\r' == c) {
        /* Skip windows file format line feeds */
      }
      else {
        func(list_ptr, &state, c, line_count, column_count, i, last_function_start);
      }
    }

    line_count += ('\n' == c);
    column_count -= (('\n' != c) ? -1 : column_count);
    if ('\n' == c) {
      last_linefeed_offset = i;
    }

    num_braces -= is_code_block_end;
    num_paren += is_left_parentheis;
    num_paren -= is_right_parentheis;
    last_last_c = last_c;
    last_c = c;
    last_in_string = in_string;
  }

  free(state.buf);

  return 0;
}

int file_init(file_t* file, const char* filename) {
  int retval = 0;
  FILE* fd;
  size_t len;
  size_t rlen;
  char* buf;

  assert((NULL != file) && "Argument 'file' must not be NULL");
  assert((NULL != filename) && "Argument 'filename' must not be NULL");

  if ((NULL == file) || (NULL == filename)) {
    return -5;
  }

  if (NULL == (fd = fopen(filename, "rb"))) {
    fprintf(stderr, "ERROR: File '%s' not found.\n", filename);
    retval = -1;
    goto fopen_failed;
  }

  if (0 == (len = fsize(fd))) {
    fprintf(stderr, "ERROR: Invalid file size of '%s'.\n", filename);
    retval = -2;
    goto fsize_failed;
  }

  if (NULL == (buf = malloc(len + 1))) {
    fprintf(stderr, "ERROR: Out of memory while reading '%s'.\n", filename);
    retval = -3;
    goto malloc_failed;
  }

  if (len != (rlen = fread(buf, sizeof(char), len, fd))) {
    if (feof(fd)) {
      fprintf(stderr, "WARNING: End of file reached before expected\n");
    }
    fprintf(stderr, "ERROR: Invalid read size while reading '%s' read %lu bytes, expected %lu.\n", filename, rlen, len);
    retval = -4;
    goto fread_failed;
  }

  file->len = rlen;
  file->buf = buf;
  file->filename = (char*)filename;

  goto normal_exit;

 fread_failed:
  free(buf);
 malloc_failed:
 normal_exit:
 fsize_failed:
  fclose(fd);
 fopen_failed:
  return retval;
}

void file_cleanup(file_t* file) {
  assert((NULL != file) && "Argument 'file' must not be NULL");
  if (NULL == file) {
    return;
  }
  if (NULL != file->buf) {
    free(file->buf);
  }
}
