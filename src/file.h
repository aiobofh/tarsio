/*
 * File-parser helper functions (very ad-hoc and need-based)
 *
 *              _______          _____ ___        ______
 *                 |      ||    |         |    | |      |
 *                 |      ||    |         |    | |      |
 *                 |   ___||___ |         |___ | |______|
 *
 *              Copyright (C) 2020 AiO Secure Teletronics
 *
 * This is where much of the dark magic of Tarsio resides. Here be
 * dragons!
 *
 * This file contains all the basics for parsing pre-procsessed C
 * source code files and gther as much information as possible about
 * what should be generated and arranged in intermediate source files.
 * There are, and probably always will be, flaws in this parser. But
 * over time it will improve. The idea is to have as few dependencies
 * to other third-party tools as possible, to make Tarsio truly
 * portable and also quick.
 *
 *  This file is part of Tarsio.
 *
 *  Tarsio is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Tarsio is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Tarsio.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

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
  int skip_until_alpha_numeric_character;
  char last_c;
  char prev_last_c;
  char* buf;
  size_t idx;
  size_t line_feeds_in_declaration;
  int done;
};
typedef struct file_parse_state_s file_parse_state_t;

#define EMPTY_FILE_PARSE_STATE {0, 0, 0, 0, 0, 0, NULL, 0, 0, 0}

/* Data-type for parser-plugin function pointers */
typedef int (*file_parse_cb_t)(void* list_ptr, file_parse_state_t* state,
                               const char c, const size_t line,
                               const size_t col, const size_t offset,
                               const size_t last_function_start,
                               const size_t last_function_line);

struct file_s {
  char* buf;
  size_t len;
  const char* filename;
};
typedef struct file_s file_t;

#define FILE_EMPTY {NULL, 0, NULL}

int file_init(file_t* file, const char* filename);
int file_parse(file_parse_cb_t func, void* list_ptr, const file_t* file,
               parse_part_t parse_part, int skip_strings);
void file_cleanup(file_t* file);

#endif
