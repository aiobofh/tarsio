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

#include "file.h"
#include "tokenizer.h"
#include "debug.h"
#include "error.h"
#include "warning.h"
#include "helpers.h"

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <string.h>

#ifndef NDEBUG
#include <assert.h>
#endif


#define return_token(type, datatype, value)                             \
  do {                                                                  \
    size_t current_line = 0;                                            \
    if (lexer->file_list.current && lexer->file_list.current->current_line) { \
      current_line = lexer->file_list.current->current_line - 1;        \
    }                                                                   \
    token_t __tok = {lexer->text_next - lexer->text_start,              \
                     (char*)lexer->text_start,                          \
                     lexer->brace_depth,                                \
                     type,                                              \
                     lexer->text_start - lexer->text,                   \
                     lexer->line,                                       \
                     lexer->column,                                     \
                     TC_NONE,                                           \
                     datatype,                                          \
                     0,                                                 \
                     NULL,                                              \
                     0,                                                 \
                     {LIST_INIT},                                       \
                     {LIST_INIT},                                       \
                     {LIST_INIT},                                       \
                     lexer->file_list.current,                          \
                     current_line};                                     \
    return __tok;                                                       \
  } while (0)
/*
                     lexer->file_list.current ? (lexer->file_list.current->current_line ? lexer->file_list.current->current_line-1 : 0) : 0}; \
*/

static void token_argument_node_delete(token_argument_node_t* node);

lexer_t lexer = {LEXER_EMPTY};

static void lexer_init(const char* begin, const char* end) {
  lexer.text_start = begin,
  lexer.text_next = begin;
  lexer.text_end = end;
  lexer.text = begin;
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
    lexer->column++;
  }
  return_token(T_INTEGER, DT_NONE, value);
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
    lexer->column++;
  }
  return_token(T_INTEGER, DT_NONE, value);
}

static token_t lex_number_oct(lexer_t* lexer) {
  unsigned long int value = 0;
  lexer->text_next += 2;
  while (1) {
    char d = *lexer->text_next;
    if ((d >= '0') && (d <= '7')) value = value * 8 + (d - '0');
    else if (d != '_') break;
    lexer->text_next++;
    lexer->column++;
  }
  return_token(T_INTEGER, DT_NONE, value);
}

static token_t lex_number_dec(lexer_t* lexer) {
  unsigned long int value = 0;
  while (1) {
    char d = *lexer->text_next;
    if (isdigit(d)) value = value * 10 + (d - '0');
    else if (d != '_') break;
    lexer->text_next++;
    lexer->column++;
  }
  return_token(T_INTEGER, DT_NONE, value);
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
  lexer->column++;
  while (1) {
    char d = *lexer->text_next;
    if ((0 == esc) && (c == d)) { lexer->text_next++; break; }
    else if ('\\' == d) { lexer->text_next++; esc = 1; continue; }
    else if ('\n' == d) {
      if (lexer->file_list.current) {
        lexer->file_list.current->current_line++;
        verbose1("Current line: %lu", lexer->file_list.current->current_line);
      }
      lexer->line++;
      lexer->column = 1;
    };
    lexer->text_next++;
    lexer->column++;
    esc = 0;
  }
  return_token(T_STRING, DT_NONE, value);
}

static token_t lex_identifier(lexer_t* lexer) {
  const char* begin = lexer->text_next;

  while (isalnum(*lexer->text_next) || (*lexer->text_next == '_')) {
    lexer->text_next++;
    lexer->column++;
  }

#define eq(N,S)                                 \
  0 == qstrncmp(begin, N, S)

  if      (eq("auto"      ,  4)) { return_token(T_AUTO,      DT_PLAIN,0); }
  else if (eq("asm"       ,  3)) { return_token(T_ASM,       DT_NONE, 0); }
  else if (eq("break"     ,  5)) { return_token(T_BREAK,     DT_NONE, 0); }
  else if (eq("case"      ,  4)) { return_token(T_CASE,      DT_NONE, 0); }
  else if (eq("catch"     ,  5)) { return_token(T_CATCH,     DT_NONE, 0); }
  else if (eq("char"      ,  4)) { return_token(T_CHAR,      DT_PLAIN,0); }
  else if (eq("class"     ,  5)) { return_token(T_CLASS,     DT_NONE, 0); }
  else if (eq("const"     ,  5)) { return_token(T_CONST,     DT_NONE, 0); }
  else if (eq("continue"  ,  8)) { return_token(T_CONTINUE,  DT_NONE, 0); }
  else if (eq("default"   ,  7)) { return_token(T_DEFAULT,   DT_NONE, 0); }
  else if (eq("delete"    ,  6)) { return_token(T_DELETE,    DT_NONE, 0); }
  else if (eq("double"    ,  6)) { return_token(T_DOUBLE,    DT_PLAIN,0); }
  else if (eq("do"        ,  2)) { return_token(T_DO,        DT_NONE, 0); }
  else if (eq("else"      ,  4)) { return_token(T_ELSE,      DT_NONE, 0); }
  else if (eq("enum"      ,  4)) { return_token(T_ENUM,      DT_NONE, 0); }
  else if (eq("extern"    ,  6)) { return_token(T_EXTERN,    DT_NONE, 0); }
  else if (eq("float"     ,  5)) { return_token(T_FLOAT,     DT_PLAIN,0); }
  else if (eq("for"       ,  3)) { return_token(T_FOR,       DT_NONE, 0); }
  else if (eq("friend"    ,  6)) { return_token(T_FRIEND,    DT_NONE, 0); }
  else if (eq("goto"      ,  4)) { return_token(T_GOTO,      DT_NONE, 0); }
  else if (eq("if"        ,  2)) { return_token(T_IF,        DT_NONE, 0); }
  else if (eq("inline"    ,  6)) { return_token(T_INLINE,    DT_NONE, 0); }
  else if (eq("__inline"  ,  8)) { return_token(T___INLINE,  DT_NONE, 0); }
  else if (eq("int"       ,  3)) { return_token(T_INT,       DT_PLAIN,0); }
  else if (eq("long"      ,  4)) { return_token(T_LONG,      DT_PLAIN,0); }
  else if (eq("new"       ,  3)) { return_token(T_NEW,       DT_NONE, 0); }
  else if (eq("operator"  ,  8)) { return_token(T_OPERATOR,  DT_NONE, 0); }
  else if (eq("private"   ,  7)) { return_token(T_PRIVATE,   DT_NONE, 0); }
  else if (eq("protected" ,  9)) { return_token(T_PROTECTED, DT_NONE, 0); }
  else if (eq("public"    ,  6)) { return_token(T_PUBLIC,    DT_NONE, 0); }
  else if (eq("register"  ,  8)) { return_token(T_REGISTER,  DT_NONE, 0); }
  else if (eq("return"    ,  6)) { return_token(T_RETURN,    DT_NONE, 0); }
  else if (eq("short"     ,  5)) { return_token(T_SHORT,     DT_PLAIN,0); }
  else if (eq("signed"    ,  6)) { return_token(T_SIGNED,    DT_PLAIN,0); }
  else if (eq("sizeof"    ,  6)) { return_token(T_SIZEOF,    DT_NONE, 0); }
  else if (eq("static"    ,  6)) { return_token(T_STATIC,    DT_NONE, 0); }
  else if (eq("struct"    ,  6)) { return_token(T_STRUCT,    DT_NONE, 0); }
  else if (eq("switch"    ,  6)) { return_token(T_SWITCH,    DT_NONE, 0); }
  else if (eq("template"  ,  8)) { return_token(T_TEMPLATE,  DT_NONE, 0); }
  else if (eq("this"      ,  4)) { return_token(T_THIS,      DT_NONE, 0); }
  else if (eq("throw"     ,  5)) { return_token(T_THROW,     DT_NONE, 0); }
  else if (eq("try"       ,  3)) { return_token(T_TRY,       DT_NONE, 0); }
  else if (eq("typedef"   ,  7)) { return_token(T_TYPEDEF,   DT_NONE, 0); }
  else if (eq("union"     ,  5)) { return_token(T_UNION,     DT_NONE, 0); }
  else if (eq("unsigned"  ,  8)) { return_token(T_UNSIGNED,  DT_PLAIN,0); }
  else if (eq("virtual"   ,  7)) { return_token(T_VIRTUAL,   DT_NONE, 0); }
  else if (eq("void"      ,  4)) { return_token(T_VOID,      DT_PLAIN,0); }
  else if (eq("volatile"  ,  8)) { return_token(T_VOLATILE,  DT_NONE, 0); }
  else if (eq("while"     ,  5)) { return_token(T_WHILE,     DT_NONE, 0); }
  else if (eq("..."       ,  3)) { return_token(T_VARIADIC,  DT_NONE, 0); }
  else {  return_token(T_IDENT, DT_NONE, ident_intern(begin, end, hash)); }
#undef eq
}

static void skip_whitespace(lexer_t* lexer) {
  while (1) {
    /* Skip space, tabs, newlines, etc */
    for (; ((text_left(lexer) > 0) && isspace(lexer->text_next[0]));
         lexer->text_next++) {
      lexer->column = (('\n' == *lexer->text_next) ? 1 : lexer->column + 1);
      lexer->line += ('\n' == *lexer->text_next);
      if (lexer->file_list.current) {
        lexer->file_list.current->current_line += ('\n' == *lexer->text_next);
      }
    }

    char c = *lexer->text_next;
    char d = text_left(lexer) >= 1 ? lexer->text_next[1] : 0;

    /* Skip line comments, i.e., "// this is a comment" */
    if ((c == '/') && (d == '/')) {
      for (; (text_left(lexer) > 0) && (lexer->text_next[0] != '\n');
           lexer->text_next++) { lexer->column++; }
      continue;
      if (lexer->file_list.current) {
        lexer->file_list.current->current_line++;
      }
      lexer->line++;
      lexer->column = 1;

    /* Skip block comments, i.e., Note that block comments can be nested. */
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

  /* Skip white spaces completely */
  while (isspace(*lexer->text_next)) {
    lexer->text_next++;
    lexer->column++;
  }

}

static inline char* file_name(const token_file_t* token_file) {
  static char buf[1024];
  memcpy(buf, token_file->name, token_file->len);
  buf[token_file->len] = '\0';
  return buf;
}


static inline size_t file_line(const token_file_t* token_file) {
  return token_file->current_line;
}

#define token_file_name(token) file_name((token)->file)

#define token_file_line(token) file_line((token)->file)

static token_t lexer_next(lexer_t* lexer) {
  skip_whitespace(lexer);

  char c = *lexer->text_next;
  char d = lexer->text_next < lexer->text_end ? *(lexer->text_next + 1) : 0;

  lexer->text_start = lexer->text_next;

  /* intergers of various formats (decimal, hexadecimal, octal) */
  if (isdigit(c))                    { return lex_number(lexer); }
  /* typically all other identifiers, like function names, keywords etc */
  else if (isalpha(c) || (c == '_')) { return lex_identifier(lexer); }
  /* strings and escaped characters in strings */
  else if (c == '"')                 { return lex_string(lexer, '"'); }
  else if (c == '\'')                { return lex_string(lexer, '\''); }
  /* equals-kind tokens, e.g., ==, !=, +=, &=, etc */
  else if ((d == '=') && ((c == '+') ||
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
    return_token(300 + c, DT_NONE, 0); /* Check the definition for tokens. */
  }
  /* ++ -- << >> */
  else if ((d == c) && ((c == '+') ||
                        (c == '-') ||
                        (c == '<') ||
                        (c == '>'))) {
    lexer->text_next++;
    lexer->column++;
    return_token(400 + c, DT_NONE, 0); /* Check the definition for tokens. */
  }
  else if (0 == qstrncmp(lexer->text_next, "...", 3)) {
    lexer->text_next += 3;
    return_token(T_VARIADIC, DT_NONE, 0);
  }
  /* Single character tokens */
  else if ((c == '(') || (c == ')') || (c == '{') || (c == '}') ||
           (c == '[') || (c == ']') || (c == '.') || (c == ';') ||
           (c == ':') || (c == ',') || (c == '~') || (c == '|') ||
           (c == '&') || (c == '+') || (c == '-') || (c == '+') ||
           (c == '*') || (c == '/') || (c == '%') || (c == '=') ||
           (c == '<') || (c == '>') || (c == '#') || (c == '?') ||
           (c == '^') || (c == '!')) {
    lexer->text_next++;
    lexer->column++;
    return_token(c, DT_NONE, 0);
  }

  warning4("Could not parse line %s:%lu '%c' (:%u)",
           file_name(lexer->file_list.current),
           file_line(lexer->file_list.current),
           c,
           lexer->line);

  lexer->text_next++;
  lexer->column++;
  return_token(T_NONE, DT_NONE, 0);
}

#define token_node_new(token) __safepcall(_token_node_new(token))
static token_node_t* _token_node_new(token_t* token) {
  enum {
    ok = 0,
    node_malloc_failed = - 1
  } retval = ok;

  token_node_t* node = node_malloc(sizeof(*node)) else ret(node_malloc_failed);

  memcpy(&node->token, token, sizeof(*token));

 node_malloc_failed:

  return (ok == retval) ? node : NULL;
}

static void token_node_delete(token_node_t* node) {
  assert(NULL != node && "node agument can not be NULL");

  list_cleanup(&node->token.usage_list, free);
  list_cleanup(&node->token.return_type_list, free);
  list_cleanup(&node->token.argument_list, token_argument_node_delete);

  free(node);
}

char* token_name(const token_t* token) {
  /* NOTE: NOT thread safe, but quick! */
  static char buf[1024];

  assert(NULL != token && "token argument can not be NULL");

  memcpy(buf, token->ptr, token->len);
  buf[token->len] = '\0';
  return buf;
}

static token_t* find_previous_token_definition(token_node_t* start,
                                               token_t* token)
{
  token_node_t* node = start;

  assert(NULL != start && "start argument can not be NULL");
  assert(NULL != token && "token argument can not be NULL");

  /* Traverse the list backwards from the last known token */
  for (; (NULL != node); node = prev(node)) {
    /* TODO: Scope symbols with braces? */
    const token_t* t = &node->token;

    if (token_is_not_an_identifier(t)) continue;
    if (token_length_mismatch(t, token)) continue;
    if (token_is_not_referencing_a_datatype_nor_a_prototype(t)) continue;

    else if (0 == memcmp(t->ptr, token->ptr, token->len)) {
      /* We seem to have found the previous token matching the current one */

      /* If the lookup was previously done return the original (top-most)
       * token with the same name as "definition" return it directly so
       * that the need for looping further back is irrelevant */
      if (t->definition) {
        return t->definition;
      }

      /* Otherwise this is considered to be the first time we found it. */
      if ((t->class != TC_DATATYPE) && (t->class != TC_FUNCTION_PROTOTYPE)) {
        return NULL;
      }
      return (token_t*)t;
    }
  }
  return NULL;
}

#define token_usage_node_new(token) __safepcall(_token_usage_node_new(token))
static token_usage_node_t* _token_usage_node_new(token_t* token) {
  /* TODO: Could be consolidated with token_type_node_new() */
  enum {
    ok = 0,
    node_malloc_failed = -1
  } retval = ok;
  token_usage_node_t* node;

  node = node_malloc(sizeof(*node)) else ret(node_malloc_failed);

  verbose5("%s:%lu node = %p, token = %p '%s'",
           token_file_name(token),
           token_file_line(token),
           (void*)node,
           (void*)token,
           token_name(token));

  node->token = token;

 node_malloc_failed:
  return (ok == retval) ? node : NULL;
}

static int parse_identifier(token_t* token, token_list_t* list) {

  /* Since we already concluded that an identifier token was found, this code
   * also looks backwards in the token-list to try to find previous usages
   * and most likely finally the real declaration or datatype definition. */

  if (lexer.attribute_scan || lexer.argument_list_scan) {
    verbose1("Skipping identifer '%s'", token_name(token));
    return -1;
  }

  verbose4("'%s' at line %u col %u offset %lu",
           token_name(token), token->line, token->column, token->offset);

  /* Replicate datatypes with the first info found searching backwards
   * NOTE: This can have scope implications */
  if (NULL == last(list)) { /* This COULD be called before any items exist */
    return -2;
  }

  return 0;
}

static int is_datatype_style_token_node(token_node_t* node) {
  return (node->token.datatype ||
          (T_AUTO == node->token.type) ||
          (T_CHAR == node->token.type) ||
          (T_DOUBLE == node->token.type) ||
          (T_FLOAT == node->token.type) ||
          (T_INT == node->token.type) ||
          (T_LONG == node->token.type) ||
          (T_SHORT == node->token.type) ||
          (T_SIGNED == node->token.type) ||
          (T_UNSIGNED == node->token.type) ||
          (T_VOID == node->token.type) ||
          (T_CONST == node->token.type) ||
          (T_STRUCT == node->token.type) ||
          /* Pointers / functino pointers */
          (T_STAR == node->token.type) ||
          /* Function pointers */
          (T_LPAREN == node->token.type) ||
          (T_RPAREN == node->token.type) ||
          /* Arrays */
          (T_LBRACKET == node->token.type) ||
          (T_RBRACKET == node->token.type) ||
          ((T_INTEGER == node->token.type) &&
           (T_LBRACKET == prev(node)->token.type)) ||
          /* Variadic */
          (T_VARIADIC == node->token.type));
}

static int is_return_type_style_token_node(token_node_t* node) {
  return (is_datatype_style_token_node(node) ||
          T_INLINE == node->token.type ||
          T_STATIC == node->token.type ||
          T_EXTERN == node->token.type);
}

#define token_type_node_new(token) __safepcall(_token_type_node_new(token))
static token_type_node_t* _token_type_node_new(token_t* token) {
  /* TODO: Could be consolidated with token_usage_node_new() */
  enum {
    ok = 0,
    node_malloc_failed = -1
  } retval = ok;

  token_type_node_t* node;

  node = node_malloc(sizeof(*node)) else ret(node_malloc_failed);

  node->token = token;

 node_malloc_failed:

  return (ok == retval) ? node : NULL;
}

#define parse_return_type(node) __safeicall(_parse_return_type(node))
static int _parse_return_type(token_node_t* node) {
  enum {
    ok = 0,
    token_type_node_new_failed = -1
  } retval = ok;

  token_type_node_t* t_node;
  token_node_t* n = prev(node);
  token_type_list_t* t_list = &node->token.return_type_list;

  debug1("Parsing return type of '%s'", token_name(&node->token));

  verbose1("Tagging the return type of '%s' (in reverse)",
           token_name(&node->token));

  for (; is_return_type_style_token_node(n); n = prev(n)) {
    token_t* token = &n->token;

    t_node = token_type_node_new(token) else ret(token_type_node_new_failed);

    verbose1("   %s", token_name(token));

    if (is_empty(t_list)) { /* If it's an empty list, first add one */
      list_add(t_list, t_node);
    }
    else { /* Otherwise prepend the list with the found node */
      list_insert(t_list, first(t_list), t_node);
    }
  }

#ifndef NODEBUG
  if (__tarsio_debug_print) {
    char return_type_raw[1024];
    size_t len = 0;
    memset(return_type_raw, 0, sizeof(return_type_raw));
    for (each(t_list, t_node)) {
      if ((len != 0) && ('*' != t_node->token->ptr[0])) {
        return_type_raw[len++] = ' ';
      }
      memcpy(&return_type_raw[len], t_node->token->ptr, t_node->token->len);
      len += t_node->token->len;
    }
    debug1("   '%s'", return_type_raw);
  }
#endif

  goto ok;

 token_type_node_new_failed:

  list_cleanup(t_list, free);

 ok:
  return retval;
}

static int parse_function_prototype(token_t* token, token_list_t* list) {
  enum {
    ok = 0,
    parse_return_type_failed = -1,
  } retval = ok;

  token_node_t* last = last(list);
  token_t* prev_token = &last->token;
  token_node_t* next_to_last = prev(last);
  token_t* prev_prev_token = &next_to_last->token;

  int datatype;

  const int too_little_history = ((NULL == prev_token) ||
                                  (NULL == prev_prev_token));

  /* Bail out if there is not enough history before the current token */
  if (too_little_history) return ok;

  /* Since this function only should be called if the current token is a
   * left-parenthesis, here is an assumption that the token left to that
   * parenthesis must be an identifier (function name). */
  if (token_is_not_an_identifier(prev_token)) return ok;

  /* It's most likely that the token left of the function name is a datatype
   * describing the return type. (or an asterisk, maybe, if the function
   * returns a pointer) */
  datatype = ((prev_prev_token->datatype) ||
              (T_AUTO == prev_prev_token->type) ||
              (T_CHAR == prev_prev_token->type) ||
              (T_DOUBLE == prev_prev_token->type) ||
              (T_FLOAT == prev_prev_token->type) ||
              (T_INT == prev_prev_token->type) ||
              (T_LONG == prev_prev_token->type) ||
              (T_SHORT == prev_prev_token->type) ||
              (T_SIGNED == prev_prev_token->type) ||
              (T_UNSIGNED == prev_prev_token->type) ||
              (T_VOID == prev_prev_token->type));

  if ((datatype) ||
      (T_CONST == prev_prev_token->type) ||
      (T_STAR == prev_prev_token->type)) {
    prev_token->function_prototype = 1;
    prev_token->class = TC_FUNCTION_PROTOTYPE;

    /* TODO: Should return type be parsed here? */
    /*
    parse_return_type(last(list)) else ret(parse_return_type_failed);
    */
    debug1("Setting '%s' as function prototype",
           token_name(prev_token));

  }

 parse_return_type_failed:

  return retval;
}

static void parse_datatype_definition(token_t* token, token_t* prev_token) {
  verbose0("Checking datatype definition");
  if (T_IDENT != token->type) {
    /* If the syntax suggests anonymous enum, union or struct, terminate
     * scanning for these types */
    if (T_ENUM == prev_token->type)        { lexer.enum_scan   = 0; }
    else if (T_UNION == prev_token->type)  { lexer.union_scan  = 0; }
    else if (T_STRUCT == prev_token->type) { lexer.struct_scan = 0; }
  }
  if (T_IDENT != prev_token->type) { /* Only continue if prev token is ident */
    return;
  }

  /* It seems the previous token was an identifier... Let's see if it's
   * followed by { ...contents... } */
  if (T_LBRACE == token->type) {
    if (lexer.enum_scan) {
      prev_token->class = TC_DATATYPE;
      prev_token->datatype = DT_ENUM;
      debug1("Setting '%s' as enum type", token_name(prev_token));
      lexer.enum_scan = 0;
    }
    else if (lexer.union_scan) {
      prev_token->class = TC_DATATYPE;
      prev_token->datatype = DT_UNION;
      debug1("Setting '%s' as union type", token_name(prev_token));
      lexer.union_scan = 0;
    }
    else if (lexer.struct_scan) {
      prev_token->class = TC_DATATYPE;
      prev_token->datatype = DT_STRUCT;
      debug1("Setting '%s' as struct type", token_name(prev_token));
      lexer.struct_scan = 0;
    }
  }
  /* Otherwise let's se if it's a forward declaration or a simple typedef */
  else if (T_SEMICOLON == token->type) {
    if (lexer.struct_scan) { /* Forward declaration */
      prev_token->class = TC_DATATYPE;
      prev_token->datatype = DT_STRUCT;
      lexer.struct_scan = 0;
    }
    if (lexer.typedef_scan) {
      prev_token->class = TC_DATATYPE;
      prev_token->datatype = DT_PLAIN;
      debug1("Setting '%s' as plan type (typedef)", token_name(prev_token));
      /* Terminate nested scan (anonymous enum, struct and union) */
      lexer.enum_scan = lexer.union_scan = lexer.struct_scan = 0;
    }
  }
}

#define token_argument_node_new_copy(node, token)               \
  __safepcall(_token_argument_node_new_copy(node, token))
static token_argument_node_t* _token_argument_node_new_copy(token_argument_node_t* node, token_t* token) {
  enum {
    ok = 0,
    node_malloc_failed = -1
  } retval = ok;
  token_argument_node_t* new_node;
  new_node = node_malloc(sizeof(*node)) else ret(node_malloc_failed);
  *new_node = *node;
  new_node->token = token;

  /* Clear the original */
  memset(node, 0, sizeof(*node));

 node_malloc_failed:
  return (ok == retval) ? new_node : NULL;

}

static void token_argument_node_delete(token_argument_node_t* node) {
  assert(NULL != node && "node agument can not be NULL");
  list_cleanup(&node->type_list, free);
  free(node);
}

#define parse_argument_list(node) __safeicall(_parse_argument_list(node))
static int _parse_argument_list(token_node_t* node) {
  enum {
    ok = 0,
    token_type_node_new_failed = -1,
    token_argument_node_new_copy_failed = -2
  } retval = ok;

  token_t* token = &node->token;
  token_node_t* n = next(next(node)); /* Skip the parenthesis */
  int paren_depth = 1;
  token_t* name_token = NULL;
  int function_pointer_mode = 0;
  int array_mode = 0;

  token_argument_node_t argument_node = {NODE_INIT,
                                         {LIST_INIT},
                                         AT_PLAIN,
                                         NULL};

  token_type_list_t* t_list = &argument_node.type_list;
  token_argument_list_t* a_list = &token->argument_list;

  debug2("Parsing arguments to '%s' (%p)", token_name(token), (void*)a_list);

  for (; paren_depth; n = next(n)) {
    paren_depth += (T_LPAREN == n->token.type);
    paren_depth -= (T_RPAREN == n->token.type);

    verbose2("   '%s' (paren_depth: %d)", token_name(&n->token), paren_depth);

    /* All datatype identifiers should be added to the type-list */
    if (((1 == paren_depth) && (T_COMMA == n->token.type)) ||
        ((0 == paren_depth) && (T_RPAREN == n->token.type))) {
      token_argument_node_t* a_node;

      if (function_pointer_mode) {
        argument_node.type = AT_FUNCTION_POINTER;
        verbose0("Exiting function pointer mode");
        function_pointer_mode = 0;
      }
      else if (array_mode) {
        argument_node.type = AT_ARRAY;
        verbose0("Exiting array mode");
        array_mode = 0;
      }

      a_node = token_argument_node_new_copy(&argument_node, name_token) else {
        ret(token_argument_node_new_copy_failed);
      }

      list_add(a_list, a_node);
#ifndef NODEBUG
      if (__tarsio_debug_print) {
        token_type_list_t* l = &a_node->type_list;
        token_type_node_t* n;
        char type_raw[1024];
        size_t len = 0;
        memset(type_raw, 0, sizeof(type_raw));
        for (each(l, n)) {
          if ((len != 0) && ('*' != n->token->ptr[0])) {
            type_raw[len++] = ' ';
          }
          memcpy(&type_raw[len], n->token->ptr, n->token->len);
          len += n->token->len;
        }
        if (len) {
          type_raw[len++] = ' ';
        }

        if (NULL == name_token) {
          debug1("Creating a new anonymous argument for '%s'",
                 token_name(&node->token));
        }
        else if (function_pointer_mode) {
          debug1("Creating a new function pointer argument for '%s'",
                 token_name(&node->token));
        }
        else if (array_mode) {
          debug1("Creating a new array argument for '%s'",
                 token_name(&node->token));
        }
        else {
          memcpy(&type_raw[len], name_token->ptr, name_token->len);
          debug2("Creating a new named argument '%s' for argument '%s'",
                 type_raw, token_name(&node->token));
        }

        verbose0("");
      }
#endif
      name_token = NULL;
    }
    /* Capture all datatype identifiers */
    else if (is_datatype_style_token_node(n) ||
        ((T_IDENT == n->token.type) && (T_IDENT == next(n)->token.type))) {
      token_type_node_t* t_node;

      t_node = token_type_node_new(&n->token) else {
        ret(token_type_node_new_failed);
      }

      if ((T_STAR == n->token.type) &&
          (T_LPAREN == prev(n)->token.type) &&
          (T_IDENT == next(n)->token.type)) {
        verbose0("Entering function pointer mode");
        function_pointer_mode = 1;
      }
      else if (T_LBRACKET == n->token.type) {
        array_mode = 1;
      }

      verbose1("   '%s' <- identifier",
               token_name(t_node->token));

      list_add(t_list, t_node);
      verbose0("")
    }
    /* Capture the argument name (if any) */
    else if ((T_IDENT == n->token.type) || (T_RPAREN == n->token.type)) {
      /* Sanity check - TODO: Remove me! */
      if (is_empty(t_list)) {
        error3("Unable to parse argument list for '%s' %s:%lu",
               token_name(&n->token),
               token_file_name(&n->token),
               token_file_line(&n->token));
        return -1;
      }

      name_token = &n->token;

      verbose1("   '%s' <- Argument name", token_name(name_token));
      verbose0("")
    }
    else if (1 == paren_depth) {
      error1("Unable to parse argument '%s'", token_name(&n->token));
      return -1;
    }
  }
  goto ok;
 token_argument_node_new_copy_failed:
 token_type_node_new_failed:

  list_cleanup(t_list, free);
  list_cleanup(a_list, token_argument_node_delete);

 ok:
  return retval;
}

static token_file_node_t* token_file_node_find(token_file_list_t* list,
                                               token_t* token)
{
  token_file_node_t* node;

  for (each(list, node)) {
    if (token->len != node->file.len) continue;
    if (0 == memcmp(token->ptr, node->file.name, token->len)) {
      return node;
    }
  }

  return NULL;
}

static size_t token2size_t(token_t* token) {
  int i;
  size_t retval = 0;

  for (i = 0; i < token->len; i++) {
    retval *= 10;
    retval += token->ptr[i] - '0';
  }

  return retval;
}

#define token_file_node_new(token, line_token) __safepcall(_token_file_node_new(token, line_token))
static token_file_node_t* _token_file_node_new(token_t* token, token_t* line_token) {
  enum {
    ok = 0,
    node_malloc_failed = -1
  } retval = ok;
  token_file_node_t* node;

  node = node_malloc(sizeof(*node)) else ret(node_malloc_failed);

  node->file.name = token->ptr;
  node->file.len = token->len;

  node->file.current_line = token2size_t(line_token);
  verbose2("Creating file node for '%s':%lu", file_name(&node->file),
           node->file.current_line);

 node_malloc_failed:

  return (ok == retval) ? node : NULL;
}

#define parse_line_directive(filename_token, line_number_token) \
  __safeicall(_parse_line_directive(filename_token, line_number_token))
static int _parse_line_directive(token_t* filename_token,
                                 token_t* line_number_token)
{
  enum {
    ok = 0,
    token_file_node_new_failed = -1
  } retval = ok;

  token_file_list_t* l = &lexer.file_list;
  token_file_node_t* f = token_file_node_find(l, filename_token);

  if (NULL == f) {
    f = token_file_node_new(filename_token, line_number_token) else {
      ret(token_file_node_new_failed);
    }

    list_add(l, f);
  }

  lexer.file_list.current = &f->file;
  lexer.file_list.current->current_line = token2size_t(line_number_token);

  verbose2("Parsing file '%s' line %lu (# line directive)",
           file_name(lexer.file_list.current),
           file_line(lexer.file_list.current));

  lexer.file_scan = 0;

 token_file_node_new_failed:

  return retval;
}

#define tokenize(list) __safeicall(_tokenize(list))
static int _tokenize(token_list_t* list) {
  enum {
    ok = 0,
    token_node_new_failed = -1,
    parse_line_directive_failed = -2,
    token_usage_node_new_failed = -3,
  } retval = ok;

  token_node_t* node;
  token_t* prev_prev_token = NULL; /* These are used for parsing, by  */
  token_t* prev_token = NULL;      /* looking backwards a few tokens. */

  while (text_left(&lexer) > 1) {
    token_t token;

    /* Fetch the next token from the source text */
    token = lexer_next(&lexer);

    /* If the found token is an identifer, a lot of conclusions can be made */
    if (T_IDENT == token.type) {
      if ((!lexer.struct_scan) && (!lexer.union_scan)) {
        if (0 == parse_identifier(&token, list)) {
          verbose1("Continuing evaluation of token '%s'", token_name(&token));
        }
      }
    }

    /* This is border-line parsing, however - we're already doing smart things
     * so why not make things easier for the prototype.c code? */

    /* For example: It's perfectly possible to figure out if we stubled upon a
     * function prototype */
    else if (T_LPAREN == token.type) {
      if (T_RPAREN == prev_prev_token->type) {
        lexer.attribute_scan = 1;
        verbose0("Entering arbitrary-compiler-secific-nonsense scanning mode");
      }
      if (0 == lexer.attribute_scan) {
        if (0 == parse_function_prototype(&token, list)) {
          lexer.argument_list_scan = 1;
          verbose0("Entering argument scanning mode (skip them)");
        }
      }
    }
    else if (T_RPAREN == token.type) {
      if ((0 != lexer.attribute_scan) && (1 == lexer.paren_depth)) {
        lexer.attribute_scan = 0;
        verbose0("Exiting arbitrary-compiler-secific-nonsense scanning mode");
      }
      if ((0 != lexer.argument_list_scan) && (1 == lexer.paren_depth)) {
        lexer.argument_list_scan = 0;
        verbose0("Exiting argument scanning mode");
      }
    }

    /* TODO: Deal with it?
     * - function pointers as typedefs
     * - arrays as typedefs
     */

    else if (T_TYPEDEF == token.type) { lexer.typedef_scan = 1; }
    else if (T_ENUM    == token.type) { lexer.enum_scan    = 1; }
    else if (T_UNION   == token.type) { lexer.union_scan   = 1; }
    else if (T_STRUCT  == token.type) { lexer.struct_scan  = 1; }
    else if ((T_STRING == token.type) && (prev_token && prev_prev_token)) {
      /* GCC style line directives */
      if ((T_CPP == prev_prev_token->type) &&
          (T_INTEGER == prev_token->type)) {
        parse_line_directive(&token, prev_token) else {
          ret(parse_line_directive_failed);
        }
      }
      /* TODO: Add other compiler specific ones, if needed.... :( */
    }

    /* It's now perfectly possible to find out if the current token is a data
     * type definition. */
    else if ((NULL != prev_token) && (0 == lexer.brace_depth)) {
      if ((T_IDENT == prev_token->type) && (0 == lexer.attribute_scan)) {
        verbose1("Parsing datatype definition for '%s'", token_name(prev_token));
        parse_datatype_definition(&token, prev_token);
      }
    }

    /* Enable datatype scanners on level 0 */
    lexer.brace_depth += (T_LBRACE == token.type);
    lexer.brace_depth -= (T_RBRACE == token.type);
    lexer.paren_depth += (T_LPAREN == token.type);
    lexer.paren_depth -= (T_RPAREN == token.type);

    prev_prev_token = prev_token;

    /* Append the freshly detected and classified token to the token-list */
    node = token_node_new(&token) else ret(token_node_new_failed);

    list_add(list, node);

    prev_token = &last(list)->token;
  }

  goto ok;

 token_node_new_failed:
 parse_line_directive_failed:
 ok:

  return retval;
}

#define classify_function_call(token) \
  __safeicall(_classify_function_call(token))
static int _classify_function_call(token_t* token) {
  enum {
    ok = 0,
    token_usage_node_new_failed = -1
  } retval = ok;

  token_usage_list_t* list = &token->definition->usage_list;
  token_usage_node_t* node;

  /* TODO: Make this C++ compatible (e.g. namespace / class) */
  const int not_inside_a_function_body = (0 == token->brace_depth);

  if (not_inside_a_function_body) return ok; /* It's fine just not here */

  token->class = TC_FUNCTION_CALL;
  token->definition->used++;

  debug4("%s:%lu '%s' %d",
         token_file_name(token),
         token_file_line(token),
         token_name(token),
         token->definition->used);

  /* For easy lookup of all usages from the definition node */
  node = token_usage_node_new(token) else ret(token_usage_node_new_failed);

  list_add(list, node);

 token_usage_node_new_failed:

  return retval;
}

static void classify_datatype_usage(token_t* token) {
  token->datatype = token->definition->datatype;
  token->class = TC_TYPE_REFERENCE;
  token->definition->used++;

  debug4("%s:%lu '%s' %d",
         token_file_name(token),
         token_file_line(token),
         token_name(token),
         token->definition->used);
}

#define classify_token_usage(list) __safeicall(_classify_token_usage(list))
static int _classify_token_usage(token_list_t* list) {
  enum {
    ok = 0,
    classify_function_call_failed = -1,
    token_usage_loop_failed = -2
  } retval = ok;

  token_node_t* node;

  for (each(list, node)) {
    token_t* token = &node->token;

    if (token_is_not_an_identifier(token)) continue;

    token->definition = find_previous_token_definition(prev(node), token);

    if (token_has_no_previous_definition(token)) continue;

    switch (token->definition->class) {
    case TC_FUNCTION_PROTOTYPE: {
      classify_function_call(token) else ret(classify_function_call_failed);
      break;
    }
    case TC_DATATYPE:
      classify_datatype_usage(token);
      break;
    default:
      warning1("Unsupported usage of '%s'", token_name(token));
      break;
    }

    continue;

  classify_function_call_failed:
    list_cleanup(&token->definition->usage_list, free); /* "neating" it :) */
    ret(token_usage_loop_failed);
  }

  goto ok;

 token_usage_loop_failed:
 ok:

  return retval;
}

#define parse_used_functions(list) __safeicall(_parse_used_functions(list))
static int _parse_used_functions(token_list_t* list) {
  enum {
    ok = 0,
    parse_return_type_failed = -1,
    parse_argument_list_failed = -2
  } retval = ok;

  token_node_t* node;

  for (each(list, node)) {
    const token_t* token = &node->token;

    if (token_is_not_a_function_prototype(token)) continue;
    if (token_is_unused(token)) continue;

    parse_return_type(node) else ret(parse_return_type_failed);
    parse_argument_list(node) else ret(parse_argument_list_failed);
  }

  goto ok;

 parse_argument_list_failed:
 parse_return_type_failed:
 ok:

  return retval;
}

static void print_prototypes(token_list_t* list) {
  token_node_t* node;

  /* Clear text log of all the symbols we care about */
  for (each(list, node)) {
    token_argument_list_t* arg_list;
    token_argument_node_t* arg_node;
    token_type_list_t* rt_list;
    token_type_node_t* rt_node;
    token_usage_list_t* usage_list = &node->token.usage_list;
    token_usage_node_t* usage_node;
    if (!node->token.used) continue; /* Only care about used tokens */
    if (!node->token.function_prototype) continue;

    if (__tarsio_verbose_print) {
      printf("%s:%lu ",
             token_file_name(&node->token), token_file_line(&node->token));
    }

    rt_list = &node->token.return_type_list;
    arg_list = &node->token.argument_list;

    for (each(rt_list, rt_node)) {
      token_type_node_t* rt_next = next(rt_node);
      if (rt_next && (T_STAR == rt_next->token->type)) {
        printf("%s", token_name(rt_node->token));
      }
      else {
        printf("%s ", token_name(rt_node->token));
      }
    }

    printf("%s(", token_name(&node->token));

    for (each(arg_list, arg_node)) {
      token_type_list_t* t_list = &arg_node->type_list;
      token_type_node_t* t_node;
      for (each(t_list, t_node)) {
        token_type_node_t* next = next(t_node);
        if (next) {
          if (T_STAR == next->token->type) {
            printf("%s", token_name(t_node->token));
          }
          else {
            printf("%s ", token_name(t_node->token));
          }
        }
        else {
          if (arg_node->token) {
            printf("%s ", token_name(t_node->token));
          }
          else {
            printf("%s", token_name(t_node->token));
          }
        }
      }
      if (NULL != arg_node->token) {
        token_argument_node_t* next = next(arg_node);
        if (next) {
          printf("%s, ", token_name(arg_node->token));
        }
        else {
          printf("%s", token_name(arg_node->token));
        }
      }
    }

    printf(");\n");

    if (__tarsio_verbose_print) {
      for (each(usage_list, usage_node)) {
        printf("   %s:%lu\n",
               token_file_name(usage_node->token),
               token_file_line(usage_node->token));
      }
    }
  }
}

int token_list_init(token_list_t* list, const file_t* file) {
  enum {
    ok = 0,
    tokenize_failed = -1,
    classify_token_usage_failed = -2,
    parse_used_functions_failed = -3
  } retval = ok;

  verbose0("token_list_init()");

  list->filename = file->filename;
  list->filesize = file->len;

  lexer_init(file->buf, file->buf + file->len);

  debug0("");
  debug0("PASS 1 - Tokenizing **********************************************");
  debug0("");

  tokenize(list) else ret(tokenize_failed);

  debug0("");
  debug0("PASS 2 - Classifying usage ***************************************");
  debug0("");

  classify_token_usage(list) else ret(classify_token_usage_failed);

  debug0("");
  debug0("PASS 3 - Parse function prototypes *******************************");
  debug0("");

  parse_used_functions(list) else ret(parse_used_functions_failed);

  debug0("");
  debug0("All relevant functions processed!");
  debug0("");

  print_prototypes(list);

 parse_used_functions_failed:
 classify_token_usage_failed:
 tokenize_failed:

  return retval;
}

static void lexer_cleanup(void) {
  list_cleanup(&lexer.file_list, free);
}

void token_list_cleanup(token_list_t* list) {
  list_cleanup(list, token_node_delete);
  lexer_cleanup();
}

/*
token_node_t* token_list_find_function_declaration(token_node_t* node) {
#define is_prototype node->token.function_prototype
#define is_used node->token.used
  for (; (node && (!is_prototype || !is_used)); node = next(node));
  debug3("Used function '%s' %d used %d times", token_name(&node->token),
         node->token.function_prototype, node->token.used);
  return node;
#undef is_used
#undef is_prototype
}
*/
static token_node_t* find_next_identifier(token_node_t* node) {
  for (each_from(node)) {
    if (token_is_not_an_identifier(&node->token)) continue;
  }
  return node;
}

token_node_t* token_list_find_next_symbol_usage(token_list_t* list,
                                                token_node_t* token_node)
{
  token_node_t* n = list->current;

  while (n) {
    token_node_t* next = next(n);

    list->brace_depth += (T_LBRACE == n->token.type);
    list->brace_depth -= (T_RBRACE == n->token.type);

    if (!list->brace_depth) goto skip_ahead;
    if ((T_IDENT == n->token.type) &&
        (NULL != next) &&
        ((T_LPAREN == next->token.type) ||
         (T_SEMICOLON == next->token.type)) && /* Function pointer assign */
        (n->token.len == token_node->token.len) &&
        (0 == strncmp(n->token.ptr, token_node->token.ptr, n->token.len))) {
      list->current = find_next_identifier(next);
      return n;
    }

  skip_ahead:

    n = next;
  }

  return NULL;
}

const token_node_t* token_list_find_beginning_of_statement(const token_node_t* n) {
  /* Rewind until last semi-colon (or right bracet) */
  for (; ((NULL != n) && ((T_SEMICOLON != n->token.type) && (T_RBRACE != n->token.type))); n = prev(n));

  if (NULL == n) return NULL;

  /* Then forward again, until we find some kind of identifier */

  /* See tokenizer.h for the 200 and 299 values */
  for (; ((NULL != n) && ((n->token.type < 200) || (n->token.type > 299))); n = next(n));

  return n;
}

const token_node_t* token_list_find_end_of_argument_list(const token_node_t* n) {
  int paren = 1;
  /* Start from the left parenthesis */
  n = next(n);

  for (; ((NULL != n) && (0 != paren)); n = next(n)) {
    paren += (T_LPAREN == n->token.type);
    paren -= (T_LPAREN == n->token.type);
  }

  return prev(n); /* Don't care about the last parenthesis */
}
