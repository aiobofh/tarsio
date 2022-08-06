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

/* Some fast string compare macros */
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
#define _q10(H,N) (_q8(&(H)[0],&(N)[0]) | _q2(&(H)[8],&(N)[8]))
#define _q11(H,N) (_q8(&(H)[0],&(N)[0]) | _q3(&(H)[8],&(N)[8]))
#define _q12(H,N) (_q8(&(H)[0],&(N)[0]) | _q4(&(H)[8],&(N)[8]))
#define _q13(H,N) (_q8(&(H)[0],&(N)[0]) | _q5(&(H)[8],&(N)[8]))
#define _q14(H,N) (_q8(&(H)[0],&(N)[0]) | _q6(&(H)[8],&(N)[8]))
#define _q15(H,N) (_q8(&(H)[0],&(N)[0]) | _q7(&(H)[8],&(N)[8]))
#define _q16(H,N) (_q8(&(H)[0],&(N)[0]) | _q8(&(H)[8],&(N)[8]))
#define _q17(H,N) (_q16(&(H)[0],&(N)[0]) | _q1(&(H)[16],&(N)[16]))
#define _q18(H,N) (_q16(&(H)[0],&(N)[0]) | _q2(&(H)[16],&(N)[16]))
#define _q19(H,N) (_q16(&(H)[0],&(N)[0]) | _q3(&(H)[16],&(N)[16]))
#define _q20(H,N) (_q16(&(H)[0],&(N)[0]) | _q4(&(H)[16],&(N)[16]))
#define _q21(H,N) (_q16(&(H)[0],&(N)[0]) | _q5(&(H)[16],&(N)[16]))
#define _q22(H,N) (_q16(&(H)[0],&(N)[0]) | _q6(&(H)[16],&(N)[16]))
#define _q23(H,N) (_q16(&(H)[0],&(N)[0]) | _q7(&(H)[16],&(N)[16]))
#define _q24(H,N) (_q16(&(H)[0],&(N)[0]) | _q8(&(H)[16],&(N)[16]))
#define _q25(H,N) (_q16(&(H)[0],&(N)[0]) | _q9(&(H)[16],&(N)[16]))
#define _q26(H,N) (_q16(&(H)[0],&(N)[0]) | _q10(&(H)[16],&(N)[16]))
#define _q27(H,N) (_q16(&(H)[0],&(N)[0]) | _q11(&(H)[16],&(N)[16]))
#define _q28(H,N) (_q16(&(H)[0],&(N)[0]) | _q12(&(H)[16],&(N)[16]))
#define _q29(H,N) (_q16(&(H)[0],&(N)[0]) | _q13(&(H)[16],&(N)[16]))
#define _q30(H,N) (_q16(&(H)[0],&(N)[0]) | _q14(&(H)[16],&(N)[16]))
#define _q31(H,N) (_q16(&(H)[0],&(N)[0]) | _q15(&(H)[16],&(N)[16]))

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

#define return_token(type, datatype, value)                      \
  do {                                                           \
    token_t __tok = {lexer->text_next - lexer->text_start,       \
                     (char*)lexer->text_start,                   \
                     type,                                       \
                     lexer->text_start - lexer->text,            \
                     lexer->line,                                \
                     lexer->column,                              \
                     datatype,                                   \
                     0,                                          \
                     NULL,                                       \
                     0,                                          \
                     TOKEN_USAGE_LIST_EMPTY,                     \
                     TOKEN_TYPE_LIST_EMPTY,                      \
                     TOKEN_ARGUMENT_LIST_EMPTY};                 \
    return __tok;                                                \
  } while (0)


lexer_t lexer;

static void lexer_new(const char* begin, const char* end) {
  lexer.line = 1;
  lexer.column = 1;
  lexer.offset = 0;
  lexer.text_start = begin,
  lexer.text_next = begin;
  lexer.text_end = end;
  lexer.text = begin;
  lexer.typedef_scan = 0;
  lexer.enum_scan = 0;
  lexer.union_scan = 0;
  lexer.struct_scan = 0;
  lexer.attribute_scan = 0;
  lexer.brace_depth = 0;
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
  while (1) {
    char d = *lexer->text_next;
    if ((0 == esc) && (c == d)) { lexer->text_next++; break; }
    else if ('\\' == d) { lexer->text_next++; esc = 1; continue; }
    else if ('\n' == d) { lexer->line++; };
    lexer->text_next++;
    esc = 0;
  }
  return_token(T_STRING, DT_NONE, value);
}

static token_t lex_identifier(lexer_t* lexer) {
  const char* begin = lexer->text_next;

  while (isalnum(*lexer->text_next) || (*lexer->text_next == '_')) {
    lexer->text_next++;
  }

#define eq(N,S)                                 \
  0 == qstrncmp(begin, N, S)

  if      (eq("auto"          ,  4)) { return_token(T_AUTO,      DT_PLAIN,0); }
  else if (eq("asm"           ,  3)) { return_token(T_ASM,       DT_NONE, 0); }
  else if (eq("break"         ,  5)) { return_token(T_BREAK,     DT_NONE, 0); }
  else if (eq("case"          ,  4)) { return_token(T_CASE,      DT_NONE, 0); }
  else if (eq("catch"         ,  5)) { return_token(T_CATCH,     DT_NONE, 0); }
  else if (eq("char"          ,  4)) { return_token(T_CHAR,      DT_PLAIN,0); }
  else if (eq("class"         ,  5)) { return_token(T_CLASS,     DT_NONE, 0); }
  else if (eq("const"         ,  5)) { return_token(T_CONST,     DT_NONE, 0); }
  else if (eq("continue"      ,  8)) { return_token(T_CONTINUE,  DT_NONE, 0); }
  else if (eq("default"       ,  7)) { return_token(T_DEFAULT,   DT_NONE, 0); }
  else if (eq("delete"        ,  6)) { return_token(T_DELETE,    DT_NONE, 0); }
  else if (eq("double"        ,  6)) { return_token(T_DOUBLE,    DT_PLAIN,0); }
  else if (eq("do"            ,  2)) { return_token(T_DO,        DT_NONE, 0); }
  else if (eq("else"          ,  4)) { return_token(T_ELSE,      DT_NONE, 0); }
  else if (eq("enum"          ,  4)) { return_token(T_ENUM,      DT_NONE, 0); }
  else if (eq("extern"        ,  6)) { return_token(T_EXTERN,    DT_NONE, 0); }
  else if (eq("float"         ,  5)) { return_token(T_FLOAT,     DT_PLAIN,0); }
  else if (eq("for"           ,  3)) { return_token(T_FOR,       DT_NONE, 0); }
  else if (eq("friend"        ,  6)) { return_token(T_FRIEND,    DT_NONE, 0); }
  else if (eq("goto"          ,  4)) { return_token(T_GOTO,      DT_NONE, 0); }
  else if (eq("if"            ,  2)) { return_token(T_IF,        DT_NONE, 0); }
  else if (eq("inline"        ,  6)) { return_token(T_INLINE,    DT_NONE, 0); }
  else if (eq("__inline"      ,  8)) { return_token(T___INLINE,  DT_NONE, 0); }
  else if (eq("int"           ,  3)) { return_token(T_INT,       DT_PLAIN,0); }
  else if (eq("long"          ,  4)) { return_token(T_LONG,      DT_PLAIN,0); }
  else if (eq("new"           ,  3)) { return_token(T_NEW,       DT_NONE, 0); }
  else if (eq("operator"      ,  8)) { return_token(T_OPERATOR,  DT_NONE, 0); }
  else if (eq("private"       ,  7)) { return_token(T_PRIVATE,   DT_NONE, 0); }
  else if (eq("protected"     ,  9)) { return_token(T_PROTECTED, DT_NONE, 0); }
  else if (eq("public"        ,  6)) { return_token(T_PUBLIC,    DT_NONE, 0); }
  else if (eq("register"      ,  8)) { return_token(T_REGISTER,  DT_NONE, 0); }
  else if (eq("return"        ,  6)) { return_token(T_RETURN,    DT_NONE, 0); }
  else if (eq("short"         ,  5)) { return_token(T_SHORT,     DT_PLAIN,0); }
  else if (eq("signed"        ,  6)) { return_token(T_SIGNED,    DT_PLAIN,0); }
  else if (eq("sizeof"        ,  6)) { return_token(T_SIZEOF,    DT_NONE, 0); }
  else if (eq("static"        ,  6)) { return_token(T_STATIC,    DT_NONE, 0); }
  else if (eq("struct"        ,  6)) { return_token(T_STRUCT,    DT_NONE, 0); }
  else if (eq("switch"        ,  6)) { return_token(T_SWITCH,    DT_NONE, 0); }
  else if (eq("template"      ,  8)) { return_token(T_TEMPLATE,  DT_NONE, 0); }
  else if (eq("this"          ,  4)) { return_token(T_THIS,      DT_NONE, 0); }
  else if (eq("throw"         ,  5)) { return_token(T_THROW,     DT_NONE, 0); }
  else if (eq("try"           ,  3)) { return_token(T_TRY,       DT_NONE, 0); }
  else if (eq("typedef"       ,  7)) { return_token(T_TYPEDEF,   DT_NONE, 0); }
  else if (eq("union"         ,  5)) { return_token(T_UNION,     DT_NONE, 0); }
  else if (eq("unsigned"      ,  8)) { return_token(T_UNSIGNED,  DT_PLAIN,0); }
  else if (eq("virtual"       ,  7)) { return_token(T_VIRTUAL,   DT_NONE, 0); }
  else if (eq("void"          ,  4)) { return_token(T_VOID,      DT_PLAIN,0); }
  else if (eq("volatile"      ,  8)) { return_token(T_VOLATILE,  DT_NONE, 0); }
  else if (eq("while"         ,  5)) { return_token(T_WHILE,     DT_NONE, 0); }
  else if (eq("..."           ,  3)) { return_token(T_VARIADIC,  DT_NONE, 0); }
  else {  return_token(T_IDENT, DT_NONE, ident_intern(begin, end, hash)); }
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
    return_token(300 + c, DT_NONE, 0); /* Check the definition for these tokens. */

    /* ++ -- << >> */
  } else if ((d == c) && ((c == '+') ||
                          (c == '-') ||
                          (c == '<') ||
                          (c == '>'))) {
    lexer->text_next++;
    return_token(400 + c, DT_NONE, 0); /* Check the definition for these tokens. */

  } else if (0 == qstrncmp(lexer->text_next, "...", 3)) {
    lexer->text_next += 3;
    return_token(T_VARIADIC, DT_NONE, 0);
  /* Single character tokens */
  } else if ((c == '(') || (c == ')') || (c == '{') || (c == '}') ||
             (c == '[') || (c == ']') || (c == '.') || (c == ';') ||
             (c == ':') || (c == ',') || (c == '~') || (c == '|') ||
             (c == '&') || (c == '+') || (c == '-') || (c == '+') ||
             (c == '*') || (c == '/') || (c == '%') || (c == '=') ||
             (c == '<') || (c == '>') || (c == '#') || (c == '?') ||
             (c == '^') || (c == '!')) {
    lexer->text_next++;
    return_token(c, DT_NONE, 0);
  }
  fprintf(stderr, "WARNING: Could not parse line %d '%c'\n", lexer->line, c);
  lexer->text_next++;
  return_token(T_NONE, DT_NONE, 0);
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

static token_t* token_list_append(token_list_t* list, token_t* token) {
  token_node_t* node = token_node_new(token);

  if (NULL == node) {
    return NULL;
  }
  if (NULL == list->first) {
    list->first = node;
  }
  if (NULL != list->last) {
    list->last->next = node;
  }
  node->prev = list->last;
  list->last = node;
  list->cnt++;
  return &node->token;
}

static void token_list_delete(token_list_t* list) {
  token_node_t* node = list->first;
  while (node) {
    token_node_t* next = node->next;
    token_node_delete(node);
    node = next;
  }
}

char* token_name(const token_t* token) {
  /* NOT thread safe, but quick! */
  static char buf[1024];
  memcpy(buf, token->ptr, token->len);
  buf[token->len] = '\0';
  return buf;
}

static token_t* find_previous_token_definition(token_node_t* start,
                                               token_t* token)
{
  /* Traverse the list backwards from the last known token */
  for (; (NULL != start); start = start->prev) {
    /* TODO: Scope symbols with braces? */
    token_t* t = &start->token;
    if (T_IDENT != t->type) continue; /* Only search for identifiers  */
    else if (t->len != token->len) continue; /* .. of the same length */
    else if (0 == memcmp(t->ptr, token->ptr, token->len)) {
      /* We seem to have found the previous token matching the current one */

      /* If the lookup was previously done return the original (top-most)
       * token with the same name as "definition" */
      if (t->definition) {
        return t->definition;
      }

      /* Otherwise this is considered to be the first time we found it. */
      return t;
    }
  }
  return NULL;
}

static token_usage_node_t* token_usage_node_new(token_t* token) {
  token_usage_node_t* node = malloc(sizeof(*node));
  if (NULL == node) {
    error0("Out of memory");
    return NULL;
  }
  node->next = node->prev = NULL;
  node->token = token;
  return node;
}

static token_usage_node_t* token_usage_list_append(token_usage_list_t* list,
                                                   token_t* token) {
  token_usage_node_t* node = token_usage_node_new(token);

  if (NULL == node) {
    return NULL;
  }
  if (NULL == list->first) {
    list->first = node;
  }
  if (NULL != list->last) {
    list->last->next = node;
  }
  node->prev = list->last;
  list->last = node;
  return node;
}

static void parse_identifier(token_t* token, token_list_t* list) {
  debug4("Found identifier: '%s' at line %u col %u offset %lu",
         token_name(token), token->line, token->column, token->offset);

  /* Since we already concluded that an identifier token was found, this code
   * also looks backwards in the token-list to try to find previous usages
   * and most likely finally the real declaration or datatype definition. */

  /* Replicate datatypes with the first info found searching backwards
   * NOTE: This can have scope implications */

  if (NULL != list->last) { /* This COULD be called before any items exist */

    token->definition = find_previous_token_definition(list->last, token);

    if (NULL != token->definition) {

      if (token->definition->datatype) {
        token->datatype = token->definition->datatype;
        token->definition->used++;
        debug3("'%s' is a type,\n"
               "\n"
               "           Originally defined at line %u,\n"
               "           Now used %d times\n",
               token_name(token),
               token->definition->line,
               token->definition->used);
      }
      else if (token->definition->function_prototype) {
        if (lexer.brace_depth) {
          token->function_prototype = token->definition->function_prototype;
          token->definition->used++;
          debug3("'%s' is a function call.\n"
                 "\n"
                 "           Originally defined at line %u,\n"
                 "           Now used %d times\n",
                 token_name(token),
                 token->definition->line,
                 token->definition->used);
        }
        else {
          token->function_prototype = token->definition->function_prototype;
          if (lexer.attribute_scan && lexer.paren_depth) {
            debug2("'%s' is a function used as __attribute__.\n"
                   "\n"
                   "           Originally defined at line %u,\n",
                   token_name(token),
                   token->definition->line);
          }
          else {
            debug2("'%s' is a function implementation.\n"
                   "\n"
                   "           Originally defined at line %u,\n"
                   "           This is where eventual extern declarations\n"
                   "           will be inserted\n",
                   token_name(token),
                   token->definition->line);
          }
        }
      }

    }
  }
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
          /* Pointers / functino pointers */
          (T_STAR == node->token.type) ||
          /* Function pointers */
          (T_LPAREN == node->token.type) ||
          (T_RPAREN == node->token.type) ||
          /* Arrays */
          (T_LBRACKET == node->token.type) ||
          (T_RBRACKET == node->token.type) ||
          ((T_INTEGER == node->token.type) &&
           (T_LBRACKET == node->prev->token.type)));
}

static int is_return_type_style_token_node(token_node_t* node) {
  return (is_datatype_style_token_node(node) ||
          T_INLINE == node->token.type ||
          T_STATIC == node->token.type ||
          T_EXTERN == node->token.type);
}

static token_type_node_t* token_type_node_new(token_t* token) {
  token_type_node_t* node = malloc(sizeof(*node));
  if (NULL == node) {
    error0("Out of memory");
    return NULL;
  }

  node->prev = node->next = NULL;
  node->token = token;
  return node;
}

static int token_type_list_prepend(token_type_list_t* list, token_t* token) {
  token_type_node_t* node = token_type_node_new(token);

  if (NULL == node) {
    return -1;
  }

  if (NULL == list->last) {
    list->last = node;
  }
  if (NULL != list->first) {
    list->first->prev = node;
  }
  node->next = list->first;
  list->first = node;
  list->cnt++;
  return 0;
}

static int token_type_list_append(token_type_list_t* list, token_t* token) {
  token_type_node_t* node = token_type_node_new(token);

  debug1("       Adding '%s'", token_name(token));

  if (NULL == node) {
    error0("Out of memory");
    return -1;
  }

  if (NULL == list->first) {
    list->first = node;
  }
  if (NULL != list->last) {
    list->last->next = node;
  }
  node->prev = list->last;
  list->last = node;
  list->cnt++;
  return 0;
}

static int parse_return_type(token_node_t* node) {
  token_node_t* n = node->prev;
  debug1("Tagging the return type of '%s'", token_name(&node->token));
  for (; is_return_type_style_token_node(n); n = n->prev) {
    debug1("   %s", token_name(&n->token));
    if (0 != token_type_list_prepend(&node->token.return_type_list, &n->token)) {
      return -1;
    }
  }
  return 0;
}

static void parse_function_prototype(token_t* token, token_list_t* list) {
  token_t* prev_token = &list->last->token;
  token_t* prev_prev_token = &list->last->prev->token;

  int datatype;

  /* Bail out if there is not enough history before the current token */
  if ((NULL == prev_token) || (NULL == prev_prev_token)) return;

  /* Since this function only should be called if the current token is a
   * left-parenthesis, here is an assumption that the token left to that
   * parenthesis must be an identifier (function name). */
  if (T_IDENT != prev_token->type) return;

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
    debug1("Setting '%s' as function prototype",
           token_name(prev_token));

    /* TODO: Should return type be parsed here? */
    if (0 != parse_return_type(list->last)) {
      error1("Could not parse return type of '%s'", token_name(prev_token));
      exit(EXIT_FAILURE);
    }
  }
}

static void parse_datatype_definition(token_t* token, token_t* prev_token) {
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
      prev_token->datatype = DT_ENUM;
      debug1("Setting '%s' as enum type", token_name(prev_token));
      lexer.enum_scan = 0;
    }
    else if (lexer.union_scan) {
      prev_token->datatype = DT_UNION;
      debug1("Setting '%s' as union type", token_name(prev_token));
      lexer.union_scan = 0;
    }
    else if (lexer.struct_scan) {
      prev_token->datatype = DT_STRUCT;
      debug1("Setting '%s' as struct type", token_name(prev_token));
      lexer.struct_scan = 0;
    }
  }
  /* Otherwise let's se if it's a forward declaration or a simple typedef */
  else if (T_SEMICOLON == token->type) {
    if (lexer.struct_scan) { /* Forward declaration */
      prev_token->datatype = DT_STRUCT;
      lexer.struct_scan = 0;
    }
    if (lexer.typedef_scan) {
      prev_token->datatype = DT_PLAIN;
      debug1("Setting '%s' as plan type (typedef)", token_name(prev_token));
      /* Terminate nested scan (anonymous enum, struct and union) */
      lexer.enum_scan = lexer.union_scan = lexer.struct_scan = 0;
    }
  }
}

static int token_argument_list_append_copy(token_argument_list_t* list,
                                           token_argument_node_t* template_node,
                                           token_t* token)
{
  token_argument_node_t* node = malloc(sizeof(*node));

  if (NULL == node) {
    error0("Out of memory");
    return -1;
  }
  memcpy(node, template_node, sizeof(*node));
  memset(template_node, 0, sizeof(*template_node));

  node->token = token;

  if (NULL == node) {
    return -2;
  }
  if (NULL == list->first) {
    list->first = node;
  }
  if (NULL != list->last) {
    list->last->next = node;
  }
  node->prev = list->last;
  list->last = node;
  list->cnt++;

  return 0;
}

static int parse_argument_list(token_node_t* node) {
  token_t* token = &node->token;
  token_node_t* n = node->next->next; /* Skip the parenthesis */
  int paren_depth = 1;

  token_argument_node_t argument_node = {NULL,
                                         NULL,
                                         TOKEN_TYPE_LIST_EMPTY,
                                         NULL};

  debug1("Parsing arguments to '%s'", token_name(token));

  for (; paren_depth; n = n->next) {
    paren_depth += (T_LPAREN == n->token.type);
    paren_depth -= (T_RPAREN == n->token.type);
    if (paren_depth == 0) break;

    if (is_datatype_style_token_node(n) || (T_VARIADIC == n->token.type)) {
      /* Add the type to the list of types for the current argument */
      debug2("   '%s' %d", token_name(&n->token), n->token.type);
      if (0 != token_type_list_append(&argument_node.type_list, &n->token)) {
        error1("Could not add datatype '%s' to argument", token_name(&n->token));
        return -1;
      }
    }
    else if (T_IDENT == n->token.type) {
      if (T_IDENT == n->next->token.type) {
        token_type_list_append(&argument_node.type_list, &n->token);
        continue;
      }
      if (!argument_node.type_list.first) {
        error1("Unable to parse argument list for '%s'",
               token_name(&n->token));
        return -1;
      }

      /* There should be a populated type list now */
      debug2("   name: '%s' %d", token_name(&n->token), n->token.type);
      if (0 != token_argument_list_append_copy(&token->argument_list,
                                               &argument_node,
                                               &n->token)) {
        error1("Unable to append argument node to '%s'",
               token_name(&n->token));
      }

      if (T_COMMA == n->next->token.type) {
        n = n->next; /* Skip the comma */
      }
    }
    else if (T_COMMA == n->token.type) {
      debug0("   anonymous argument name...");
      token_argument_list_append_copy(&token->argument_list,
                                      &argument_node,
                                      NULL);
    }
    else {
      error1("Unable to parse argument '%s'", token_name(&n->token));
      return -1;
    }
  }
  return 0;
}

int token_list_init(token_list_t* list, const file_t* file) {
  token_t* prev_prev_token = NULL; /* These are used for parsing, by  */
  token_t* prev_token = NULL;      /* looking backwards a few tokens. */
  list->filename = file->filename;
  list->filesize = file->len;
  lexer_new(file->buf, file->buf + file->len);

  while (text_left(&lexer) > 1) {
    token_t token;

    /* Fetch the next token from the source text */
    token = lexer_next(&lexer);

    /* If the found token is an identifer, a lot of conclusions can be made */
    if (T_IDENT == token.type) {
      parse_identifier(&token, list);
    }

    /* This is border-line parsing, however - we're already doing smart things
     * so why not make things easier for the prototype.c code? */

    /* It's perfectly possible to figure out if we stubled upon a function
     * prototype */
    else if (T_LPAREN == token.type) {
      if (T_RPAREN == prev_prev_token->type) {
        lexer.attribute_scan = 1;
        debug0("Entering __attribute__ scanning mode");
      }
      if (0 == lexer.attribute_scan) {
        parse_function_prototype(&token, list);
      }
    }
    else if (T_RPAREN == token.type) {
      if ((0 != lexer.attribute_scan) && (1 == lexer.paren_depth)) {
        lexer.attribute_scan = 0;
        debug0("Exiting __attribute__ scanning mode");
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

    /* It's now perfectly possible to find out if the current token is a data
     * type definition. */
    else if ((NULL != prev_token) && (0 == lexer.brace_depth)) {
      parse_datatype_definition(&token, prev_token);
    }

    /* Enable datatype scanners on level 0 */
    lexer.brace_depth += (T_LBRACE == token.type);
    lexer.brace_depth -= (T_RBRACE == token.type);
    lexer.paren_depth += (T_LPAREN == token.type);
    lexer.paren_depth -= (T_RPAREN == token.type);

    prev_prev_token = prev_token;

    /* Append the freshly detected and classified token to the token-list */
    prev_token = token_list_append(list, &token);

    if (NULL == prev_token) {
      error0("Could not append token to list");
      return -1;
    }

    /* DO STUFF HERE!!!!!! */
    if (prev_token->definition) {
      token_usage_list_t* usage_list = &prev_token->definition->usage_list;
      if (NULL == token_usage_list_append(usage_list, prev_token)) {
        error0("Could not append symbol usage to usage list");
        return -2;
      }
    }
  }

  token_node_t* node;
  /* Second pass - find all the function arguments for each used function */
  for (node = list->first; node; node = node->next) {
    if (!node->token.used) continue; /* Only care about used tokens */
    if (!node->token.function_prototype) continue;

    if (0 != parse_argument_list(node)) {
      error1("Unable to create a list of arguments for '%s'",
             token_name(&node->token));
    }
  }

  /* Clear text log of all the symbols we care about */
  for (node = list->first; node; node = node->next) {
    token_argument_list_t* arg_list;
    token_argument_node_t* arg_node;
    token_type_list_t* rt_list;
    token_type_node_t* rt_node;
    if (!node->token.used) continue; /* Only care about used tokens */
    if (!node->token.function_prototype) continue;

    rt_list = &node->token.return_type_list;
    arg_list = &node->token.argument_list;

    for (rt_node = rt_list->first; rt_node; rt_node = rt_node->next) {
      if (rt_node->next && (T_STAR == rt_node->next->token->type)) {
        printf("%s", token_name(rt_node->token));
      }
      else {
        printf("%s ", token_name(rt_node->token));
      }
    }

    printf("%s(", token_name(&node->token));

    for (arg_node = arg_list->first; arg_node; arg_node = arg_node->next) {
      token_type_list_t* t_list = &arg_node->type_list;
      token_type_node_t* t_node;
      for (t_node = t_list->first; t_node; t_node = t_node->next) {
        if (T_VARIADIC == t_node->token->type) {
          debug0("IT's in there!!!");
        }
        if (t_node->next) {
          if (T_STAR == t_node->next->token->type) {
            printf("%s(%d)", token_name(t_node->token), t_node->token->type);
          }
          else {
            printf("%s(%d) ", token_name(t_node->token), t_node->token->type);
          }
        }
        else {
          if (arg_node->token) {
            printf("%s(%d) ", token_name(t_node->token), t_node->token->type);
          }
          else {
            printf("%s(%d)", token_name(t_node->token), t_node->token->type);
          }
        }
      }
      if (NULL != arg_node->token) {
        if (arg_node->next) {
          printf("%s, ", token_name(arg_node->token));
        }
        else {
          printf("%s", token_name(arg_node->token));
        }
      }
      else {
        printf(", ");
      }
    }

    printf(");\n");
  }


  return 0;
}

void token_list_cleanup(token_list_t* list) {
  token_list_delete(list);
}

token_node_t* token_list_find_function_declaration(token_node_t* node) {
#define is_prototype node->token.function_prototype
#define is_used node->token.used
  for (; (node && (!is_prototype || !is_used)); node = node->next);
  debug3("Used function '%s' %d used %d times", token_name(&node->token),
         node->token.function_prototype, node->token.used);
  return node;
#undef is_used
#undef is_prototype
}

static token_node_t* find_next_identifier(token_node_t* node) {
  for (; ((!node) && (T_IDENT != node->token.type)); node = node->next);
  return node;
}

token_node_t* token_list_find_next_symbol_usage(token_list_t* list,
                                                token_node_t* token_node)
{
  token_node_t* n = list->current;
  while (n) {
    list->brace_depth += (T_LBRACE == n->token.type);
    list->brace_depth -= (T_RBRACE == n->token.type);
    if (!list->brace_depth) goto skip_ahead;
    if ((T_IDENT == n->token.type) &&
        (NULL != n->next) &&
        ((T_LPAREN == n->next->token.type) ||
         (T_SEMICOLON == n->next->token.type)) && /* Function pointer assign */
        (n->token.len == token_node->token.len) &&
        (0 == strncmp(n->token.ptr, token_node->token.ptr, n->token.len))) {
      list->current = find_next_identifier(n->next);
      return n;
    }
  skip_ahead:
    n = n->next;
  }
  return NULL;
}

const token_node_t* token_list_find_beginning_of_statement(const token_node_t* n) {
  /* Rewind until last semi-colon (or right bracet) */
  for (; ((NULL != n) && ((T_SEMICOLON != n->token.type) && (T_RBRACE != n->token.type))); n = n->prev);

  if (NULL == n) return NULL;

  /* Then forward again, until we find some kind of identifier */

  /* See tokenizer.h for the 200 and 299 values */
  for (; ((NULL != n) && ((n->token.type < 200) || (n->token.type > 299))); n = n->next);

  return n;
}

const token_node_t* token_list_find_end_of_argument_list(const token_node_t* n) {
  int paren = 1;
  /* Start from the left parenthesis */
  n = n->next;

  for (; ((NULL != n) && (0 != paren)); n = n->next) {
    paren += (T_LPAREN == n->token.type);
    paren -= (T_LPAREN == n->token.type);
  }

  return n->prev; /* Don't care about the last parenthesis */
}
