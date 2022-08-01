/*
 * Tokenizer helper functions
 *
 *              _______          _____ ___        ______
 *                 |      ||    |         |    | |      |
 *                 |      ||    |         |    | |      |
 *                 |   ___||___ |         |___ | |______|
 *
 *            Copyright (C) 2020-2022 AiO Secure Teletronics
 *
 * This file contains all the code to tokenize a C (or C++) file to be
 * used by the prototype detector.
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
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <string.h>

#ifndef NDEBUG
#include <assert.h>
#endif

#include "file.h"
#include "tokenizer.h"
#include "debug.h"
#include "error.h"

#if UINTPTR_MAX == 0xffffffff
#else
#define Q64BIT
#endif

#define U08 uint8_t
#define U16 uint16_t
#define U32 uint32_t
#ifdef Q64BIT
#define U64 uint64_t
#endif

#define _q1(H,N) ((*(U08*)&(H)[0] ^ *(U08*)&(N)[0]))
#define _q2(H,N) ((*(U16*)&(H)[0] ^ *(U16*)&(N)[0]))
#define _q3(H,N) (_q2(&(H)[0],&(N)[0]) | _q1(&(H)[2],&(N)[2]))
#define _q4(H,N) ((*(U32*)&(H)[0] ^ *(U32*)&(N)[0]))
#define _q5(H,N) (_q4(&(H)[0],&(N)[0]) | _q1(&(H)[4],&(N)[4]))
#define _q6(H,N) (_q4(&(H)[0],&(N)[0]) | _q2(&(H)[4],&(N)[4]))
#define _q7(H,N) (_q4(&(H)[0],&(N)[0]) | _q3(&(H)[4],&(N)[4]))
#ifdef Q64BIT
#define _q8(H,N) ((*(U64*)&(H)[0] ^ *(U64*)&(N)[0]))
#else
#define _q8(H,N) (_q4(&(H)[0],&(N)[0]) | _q4(&(H)[4],&(N)[4]))
#endif
#define _q9(H,N) (_q8(&(H)[0],&(N)[0]) | _q1(&(H)[8],&(N)[8]))

#ifndef NDEBUG
#define _qstrncmp_assert(S,N) | qstrncmp_assert(S,N)
#else
#define _qstrncmp_assert(S,N)
#endif
#define qstrncmp(H,N,S) (_q##S(H,N) _qstrncmp_assert(sizeof(N) - 1 - S, N))
#ifndef NDEBUG
#ifndef STRNCMP_ASSERT
#define STRNCMP_ASSERT
static inline int qstrncmp_assert(int s, const char* needle) {
  if (s) {
    error1("Wrong size specified for '%s'", needle);
  }
  assert(!s && "Wrong constant size");
  return 0;
}
#else
extern static inline int qstrncmp_assert(int s);
#endif
#endif

#define return_token(type, dst, value)                           \
  do {                                                           \
    token_t __tok = {lexer->text_next - lexer->text_start,       \
                     (char*)lexer->text_start,                   \
                     type,                                       \
                     lexer->offset,                              \
                     lexer->line,                                \
                     lexer->column};                             \
    return __tok;                                                \
  } while (0)


lexer_t lexer;

static void lexer_new(const char* begin, const char* end) {
  lexer.line = 1;
  lexer.column = 1;
  lexer.text_start = begin,
  lexer.text_next = begin;
  lexer.text_end = end;
}

static size_t text_left(lexer_t* lexer) {
  return lexer->text_end - lexer->text_next;
}

static token_t lex_number_hex(lexer_t* lexer) {
  lexer->text_next += 2;
  unsigned long int value = 0;
  while (1) {
    char d = *lexer->text_next;
    if (isdigit(d)) value = value * 16 + (d - '0');
    else if ((d >= 'a') && (d <= 'f')) value = value * 16 + (d - 'a' + 10);
    else if ((d >= 'A') && (d <= 'F')) value = value * 16 + (d - 'A' + 10);
    else if (d != '_')  break;
    lexer->text_next++;
  }
  return_token(T_INTEGER, i, value);
}

static token_t lex_number_bin(lexer_t* lexer) {
  lexer->text_next += 2;
  unsigned long int value = 0;
  while (1) {
    char d = *lexer->text_next;
    if (d == '1') value = value * 2 + 1;
    else if (d == '0') value = value * 2 + 0;
    else if (d != '_')  break;
    lexer->text_next++;
  }
  return_token(T_INTEGER, i, value);
}

static token_t lex_number_oct(lexer_t* lexer) {
  unsigned long int value = 0;
  lexer->text_next += 2;
  while (1) {
    char d = *lexer->text_next;
    if ((d >= '0') && (d <= '7')) value = value * 8 + (d - '0');
    else if (d != '_') break;
    lexer->text_next++;
  }
  return_token(T_INTEGER, i, value);
}

static token_t lex_number_dec(lexer_t* lexer) {
  unsigned long int value = 0;
  while (1) {
    char d = *lexer->text_next;
    if (isdigit(d)) value = value * 10 + (d - '0');
    else if (d != '_') break;
    lexer->text_next++;
  }
  return_token(T_INTEGER, i, value);
}

static token_t lex_number(lexer_t* lexer) {
  char type = lexer->text_next[1];
  if (type == 'x') return lex_number_hex(lexer);
  else if (type == 'b') return lex_number_bin(lexer);
  else if (type == 'o') return lex_number_oct(lexer);
  return lex_number_dec(lexer);
}


static token_t lex_string(lexer_t* lexer, char c) {
  int esc = 0;
  lexer->text_next++;
  while (1) {
    char d = *lexer->text_next;
    if ((0 == esc) && (c == d)) { lexer->text_next++; break; }
    else if ('\\' == d) { lexer->text_next++; esc = 1; continue; }
    else if ('\n' == d) { lexer->line++; };
    lexer->text_next++;
    esc = 0;
  }
  return_token(T_STRING, i, value);
}

static token_t lex_identifier(lexer_t* lexer) {
  const char* begin = lexer->text_next;

  while (isalnum(*lexer->text_next) || (*lexer->text_next == '_')) {
    lexer->text_next++;
  }

#define eq(N,S)                                 \
  0 == qstrncmp(begin, N, S)

  if      (eq("auto"     , 4)) { return_token(T_AUTO,      i ,0); }
  else if (eq("asm"      , 3)) { return_token(T_ASM,       i, 0); }
  else if (eq("break"    , 5)) { return_token(T_BREAK,     i, 0); }
  else if (eq("case"     , 4)) { return_token(T_CASE,      i, 0); }
  else if (eq("catch"    , 5)) { return_token(T_CATCH,     i, 0); }
  else if (eq("char"     , 4)) { return_token(T_CHAR,      i, 0); }
  else if (eq("class"    , 5)) { return_token(T_CLASS,     i, 0); }
  else if (eq("const"    , 5)) { return_token(T_CONST,     i, 0); }
  else if (eq("continue" , 8)) { return_token(T_CONTINUE,  i, 0); }
  else if (eq("default"  , 7)) { return_token(T_DEFAULT,   i, 0); }
  else if (eq("delete"   , 6)) { return_token(T_DELETE,    i, 0); }
  else if (eq("do"       , 2)) { return_token(T_DO,        i, 0); }
  else if (eq("double"   , 6)) { return_token(T_DOUBLE,    i, 0); }
  else if (eq("else"     , 4)) { return_token(T_ELSE,      i, 0); }
  else if (eq("enum"     , 4)) { return_token(T_ENUM,      i, 0); }
  else if (eq("extern"   , 6)) { return_token(T_EXTERN,    i, 0); }
  else if (eq("float"    , 5)) { return_token(T_FLOAT,     i, 0); }
  else if (eq("for"      , 3)) { return_token(T_FOR,       i, 0); }
  else if (eq("friend"   , 6)) { return_token(T_FRIEND,    i, 0); }
  else if (eq("goto"     , 4)) { return_token(T_GOTO,      i, 0); }
  else if (eq("if"       , 2)) { return_token(T_IF,        i, 0); }
  else if (eq("inline"   , 6)) { return_token(T_INLINE,    i, 0); }
  else if (eq("__inline" , 8)) { return_token(T___INLINE,  i, 0); }
  else if (eq("int"      , 3)) { return_token(T_INT,       i, 0); }
  else if (eq("long"     , 4)) { return_token(T_LONG,      i, 0); }
  else if (eq("new"      , 3)) { return_token(T_NEW,       i, 0); }
  else if (eq("operator" , 8)) { return_token(T_OPERATOR,  i, 0); }
  else if (eq("private"  , 7)) { return_token(T_PRIVATE,   i, 0); }
  else if (eq("protected", 9)) { return_token(T_PROTECTED, i, 0); }
  else if (eq("public"   , 6)) { return_token(T_PUBLIC,    i, 0); }
  else if (eq("register" , 8)) { return_token(T_REGISTER,  i, 0); }
  else if (eq("return"   , 6)) { return_token(T_RETURN,    i, 0); }
  else if (eq("short"    , 5)) { return_token(T_SHORT,     i, 0); }
  else if (eq("signed"   , 6)) { return_token(T_SIGNED,    i, 0); }
  else if (eq("sizeof"   , 6)) { return_token(T_SIZEOF,    i, 0); }
  else if (eq("static"   , 6)) { return_token(T_STATIC,    i, 0); }
  else if (eq("struct"   , 6)) { return_token(T_STRUCT,    i, 0); }
  else if (eq("switch"   , 6)) { return_token(T_SWITCH,    i, 0); }
  else if (eq("template" , 8)) { return_token(T_TEMPLATE,  i, 0); }
  else if (eq("this"     , 4)) { return_token(T_THIS,      i, 0); }
  else if (eq("throw"    , 5)) { return_token(T_THROW,     i, 0); }
  else if (eq("try"      , 3)) { return_token(T_TRY,       i, 0); }
  else if (eq("typedef"  , 7)) { return_token(T_TYPEDEF,   i, 0); }
  else if (eq("union"    , 5)) { return_token(T_UNION,     i, 0); }
  else if (eq("unsigned" , 8)) { return_token(T_UNSIGNED,  i, 0); }
  else if (eq("virtual"  , 7)) { return_token(T_VIRTUAL,   i, 0); }
  else if (eq("void"     , 4)) { return_token(T_VOID,      i, 0); }
  else if (eq("volatile" , 8)) { return_token(T_VOLATILE,  i, 0); }
  else if (eq("while"    , 5)) { return_token(T_WHILE,     i, 0); }
  else {  return_token(T_IDENT, ident, ident_intern(begin, end, hash)); }
#undef eq
}

static void skip_whitespace(lexer_t* lexer) {
  while (1) {
    // Skip space, tabs, newlines, etc
    for (; ((text_left(lexer) > 0) && isspace(lexer->text_next[0]));
         lexer->text_next++) { lexer->line += ('\n' == *lexer->text_next); }

    char c = *lexer->text_next;
    char d = text_left(lexer) >= 1 ? lexer->text_next[1] : 0;

    // Skip line comments, i.e., "// this is a comment"
    if ((c == '/') && (d == '/')) {
      for (; (text_left(lexer) > 0) && (lexer->text_next[0] != '\n');
           lexer->text_next++) {}
      continue;
      lexer->line++;

      // Skip block comments, i.e., "/* this is a comment */"
      // Note that block comments can be nested.
    } else if ((c == '/') && (d == '*')) {
      int nest_count = 1;
      lexer->text_next += 2;
      while ((nest_count > 0) && text_left(lexer) > 2) {
        int is_endblock = ((lexer->text_next[0] == '*') &&
                           (lexer->text_next[1] == '/'));
        int is_beginblock = ((lexer->text_next[0] == '/') &&
                             (lexer->text_next[1] == '*'));
        lexer->line += ('\n' == *lexer->text_next);
        if (is_beginblock) {
          nest_count++;
          lexer->text_next += 2;
        } else if (is_endblock) {
          nest_count--;
          lexer->text_next += 2;
        } else {
          lexer->text_next += 1;
        }
      }
      continue;
    }

    break;
  }


  while (isspace(*lexer->text_next)) {
    lexer->text_next++;
  }

}

static token_t lexer_next(lexer_t* lexer) {
  skip_whitespace(lexer);

  char c = *lexer->text_next;
  char d = lexer->text_next < lexer->text_end ? *(lexer->text_next + 1) : 0;

  lexer->text_start = lexer->text_next;

  if (isdigit(c)) {
    return lex_number(lexer);
  } else if (isalpha(c) || (c == '_')) {
    return lex_identifier(lexer);

    /* equals-kind tokens, e.g., ==, !=, +=, &=, etc */
  } else if (c == '"') {
    return lex_string(lexer, '"');
  } else if (c == '\'') {
    return lex_string(lexer, '\'');
  } else if ((d == '=') && ((c == '+') ||
                            (c == '-') ||
                            (c == '*') ||
                            (c == '/') ||
                            (c == '%') ||
                            (c == '!') ||
                            (c == '=') ||
                            (c == '|') ||
                            (c == '&') ||
                            (c == '^'))) {
    lexer->text_next += 2;
    return_token(300 + c, i, 0); /* Check the definition for these tokens. */

    /* ++ -- << >> */
  } else if ((d == c) && ((c == '+') ||
                          (c == '-') ||
                          (c == '<') ||
                          (c == '>'))) {
    lexer->text_next++;
    return_token(400 + c, i, 0); /* Check the definition for these tokens. */

    /* Single character tokens */
  } else if ((c == '(') || (c == ')') || (c == '{') || (c == '}') ||
             (c == '[') || (c == ']') || (c == '.') || (c == ';') ||
             (c == ':') || (c == ',') || (c == '~') || (c == '|') ||
             (c == '&') || (c == '+') || (c == '-') || (c == '+') ||
             (c == '*') || (c == '/') || (c == '%') || (c == '=') ||
             (c == '<') || (c == '>') || (c == '#') || (c == '?') ||
             (c == '^') || (c == '!')) {
    lexer->text_next++;
    return_token(c, i, 0);
  }
  fprintf(stderr, "WARNING: Could not parse line %d '%c'\n", lexer->line, c);
  lexer->text_next++;
  return_token(T_NONE, i, 0);
}

static token_node_t* token_node_new(token_t* token) {
  token_node_t* node;

  if (NULL == (node = malloc(sizeof(*node)))) {
    error0("Out of memory");
    goto malloc_failed;
  }

  memcpy(&node->token, token, sizeof(*token));

 malloc_failed:
  return node;
}

static void token_node_delete(token_node_t* node) {
  free(node);
}

static void token_list_append(token_list_t* list, token_t* token) {
  token_node_t* node = token_node_new(token);

  if (NULL == node) {
    return;
  }
  if (NULL == list->first) {
    list->first = node;
  }
  if (NULL != list->last) {
    list->last->next = node;
  }
  node->prev = list->last;
  list->last = node;
}

static void token_list_delete(token_list_t* list) {
  token_node_t* node = list->first;
  while (node) {
    token_node_t* next = node->next;
    token_node_delete(node);
    node = next;
  }
}

int token_list_init(token_list_t* list, file_t* file) {
  list->filename = file->filename;
  list->filesize = file->len;
  lexer_new(file->buf, file->buf + file->len);

  while (text_left(&lexer) > 1) {
    token_t token;
    token = lexer_next(&lexer);
    if (T_IDENT == token.type) {
      char* buf = malloc(token.len + 1);
      memcpy(buf, token.ptr, token.len);
      buf[token.len] = '\0';
      debug1("Found identifier: '%s'", buf);
      free(buf);
    }
    token_list_append(list, &token);
  }

  return 0;
}

void token_list_cleanup(token_list_t* list) {
  token_list_delete(list);
}

static token_node_t* parse_identifier(token_node_t* node) {
  token_node_t* next = node->next;
  token_node_t* prev = node->prev;

  /* A function name must be surrounded by tokens */
  if (NULL == next) return NULL;
  if (NULL == prev) return NULL;

  /* A function name mist be followed by a right parenthesis */
  if (T_LPAREN == next->token.type ){
    if (T_RPAREN == prev->token.type) return NULL;
    return node;
  }
  return NULL;
}

token_node_t* token_list_find_function_declaration(token_node_t* node) {
  int waiting_for = 0;
  int brace_depth = 0;
  int paren_depth = 0;
  while (node) {
    /* TODO: Braces might be inside a namespace or a class in C++ */
    brace_depth += (T_LBRACE == node->token.type);
    brace_depth -= (T_RBRACE == node->token.type);
    paren_depth += (T_LPAREN == node->token.type);
    paren_depth -= (T_RPAREN == node->token.type);
    if (brace_depth || paren_depth) {
      goto skip_ahead;
    }
    if ((waiting_for) && (waiting_for != node->token.type)) {
      goto skip_ahead;
    }
    if (T_TYPEDEF == node->token.type) {
      waiting_for = T_SEMICOLON;
    }
    else {
      waiting_for = 0;
    }

    /* Usually function signatures has a datatype identifier in-front the
     * function name */
    if (T_IDENT == node->token.type) {
      token_node_t* start_node;
      if (NULL != (start_node = parse_identifier(node))) {
        return node;
      }
    }
  skip_ahead:
    node = node->next;
  }
  return NULL;
}

token_node_t* token_list_find_function_usage(token_list_t* list, token_node_t* node) {
  return NULL;
}

/*
int main(int argc, char* argv[]) {
   FILE *fd;
   size_t size;
   char* buf;
   token_list_t list = {0, NULL, NULL};

   if (argc != 2) {
     fprintf(stderr, "ERROR: Too few arguments\n");
     exit(EXIT_FAILURE);
   }

   if (NULL == (fd = fopen(argv[1], "rb"))) {
     fprintf(stderr, "ERROR: Could not open %s\n", argv[1]);
     exit(EXIT_FAILURE);
   }

   size = fsize(fd);

   if (NULL == (buf = malloc(size))) {
     fprintf(stderr, "ERROR: Out of memory\n");
     fclose(fd);
     exit(EXIT_FAILURE);
   }

   fread(buf, 1, size, fd);

   fclose(fd);

   lexer_new(buf, buf + size);

   while (text_left(&lexer) > 1) {
     char b[1024];
     token_t token;
     token = lexer_next(&lexer);

     token_list_append(&list, &token);
   }

   token_list_delete(&list);

   free(buf);
 }
*/
