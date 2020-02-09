#ifndef _FILE_H_
#define _FILE_H_

#include <stdlib.h>

#define MAX_PROTOTYPE_BUF 1024

enum parse_part_e {
  PARSE_DECLARATIONS,
  PARSE_FUNCTION_BODIES,
  PARSE_ALL
};
typedef enum parse_part_e parse_part_t;

struct file_parse_state_s {
  int paren_count;
  int found_parenthesis;
  int skip_until_linefeed;
  char last_c;
  char prev_last_c;
  char* buf;
  size_t idx;
  size_t line_feeds_in_declaration;
  int done;
};
typedef struct file_parse_state_s file_parse_state_t;

#define EMPTY_FILE_PARSE_STATE {0, 0, 0, 0, 0, NULL, 0, 0, 0}

typedef int (*file_parse_cb_t)(void* list_ptr, file_parse_state_t* state, const char c, const size_t line ,const size_t col, const size_t offset, const size_t last_function_start);

struct file_s {
  char* buf;
  size_t len;
  char* filename;
};
typedef struct file_s file_t;

#define FILE_EMPTY {NULL, 0, NULL}

int file_init(file_t* file, const char* filename);
int file_parse(file_parse_cb_t func, void* list_ptr, const file_t* file, parse_part_t parse_part, int skip_strings);
void file_cleanup(file_t* file);

#endif
