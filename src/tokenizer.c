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
#define _q10(H,N) (_q8(&(H)[0],&(N)[0]) | _q2(&(H)[8],&(N)[8]))

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
                     lexer->text_start - lexer->text,            \
                     lexer->line,                                \
                     lexer->column,                              \
                     DT_NONE,                                    \
                     0,                                          \
                     NULL};                                      \
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

  if      (eq("auto"      , 4)) { return_token(T_AUTO,      i ,0); }
  else if (eq("asm"       , 3)) { return_token(T_ASM,       i, 0); }
  else if (eq("break"     , 5)) { return_token(T_BREAK,     i, 0); }
  else if (eq("case"      , 4)) { return_token(T_CASE,      i, 0); }
  else if (eq("catch"     , 5)) { return_token(T_CATCH,     i, 0); }
  else if (eq("char"      , 4)) { return_token(T_CHAR,      i, 0); }
  else if (eq("class"     , 5)) { return_token(T_CLASS,     i, 0); }
  else if (eq("const"     , 5)) { return_token(T_CONST,     i, 0); }
  else if (eq("continue"  , 8)) { return_token(T_CONTINUE,  i, 0); }
  else if (eq("default"   , 7)) { return_token(T_DEFAULT,   i, 0); }
  else if (eq("delete"    , 6)) { return_token(T_DELETE,    i, 0); }
  else if (eq("double"    , 6)) { return_token(T_DOUBLE,    i, 0); }
  else if (eq("do"        , 2)) { return_token(T_DO,        i, 0); }
  else if (eq("else"      , 4)) { return_token(T_ELSE,      i, 0); }
  else if (eq("enum"      , 4)) { return_token(T_ENUM,      i, 0); }
  else if (eq("extern"    , 6)) { return_token(T_EXTERN,    i, 0); }
  else if (eq("float"     , 5)) { return_token(T_FLOAT,     i, 0); }
  else if (eq("for"       , 3)) { return_token(T_FOR,       i, 0); }
  else if (eq("friend"    , 6)) { return_token(T_FRIEND,    i, 0); }
  else if (eq("goto"      , 4)) { return_token(T_GOTO,      i, 0); }
  else if (eq("if"        , 2)) { return_token(T_IF,        i, 0); }
  else if (eq("inline"    , 6)) { return_token(T_INLINE,    i, 0); }
  else if (eq("__inline"  , 8)) { return_token(T___INLINE,  i, 0); }
  else if (eq("int"       , 3)) { return_token(T_INT,       i, 0); }
  else if (eq("long"      , 4)) { return_token(T_LONG,      i, 0); }
  else if (eq("new"       , 3)) { return_token(T_NEW,       i, 0); }
  else if (eq("operator"  , 8)) { return_token(T_OPERATOR,  i, 0); }
  else if (eq("private"   , 7)) { return_token(T_PRIVATE,   i, 0); }
  else if (eq("protected" , 9)) { return_token(T_PROTECTED, i, 0); }
  else if (eq("public"    , 6)) { return_token(T_PUBLIC,    i, 0); }
  else if (eq("register"  , 8)) { return_token(T_REGISTER,  i, 0); }
  else if (eq("return"    , 6)) { return_token(T_RETURN,    i, 0); }
  else if (eq("short"     , 5)) { return_token(T_SHORT,     i, 0); }
  else if (eq("signed"    , 6)) { return_token(T_SIGNED,    i, 0); }
  else if (eq("sizeof"    , 6)) { return_token(T_SIZEOF,    i, 0); }
  else if (eq("static"    , 6)) { return_token(T_STATIC,    i, 0); }
  else if (eq("struct"    , 6)) { return_token(T_STRUCT,    i, 0); }
  else if (eq("switch"    , 6)) { return_token(T_SWITCH,    i, 0); }
  else if (eq("template"  , 8)) { return_token(T_TEMPLATE,  i, 0); }
  else if (eq("this"      , 4)) { return_token(T_THIS,      i, 0); }
  else if (eq("throw"     , 5)) { return_token(T_THROW,     i, 0); }
  else if (eq("try"       , 3)) { return_token(T_TRY,       i, 0); }
  else if (eq("typedef"   , 7)) { return_token(T_TYPEDEF,   i, 0); }
  else if (eq("union"     , 5)) { return_token(T_UNION,     i, 0); }
  else if (eq("unsigned"  , 8)) { return_token(T_UNSIGNED,  i, 0); }
  else if (eq("virtual"   , 7)) { return_token(T_VIRTUAL,   i, 0); }
  else if (eq("void"      , 4)) { return_token(T_VOID,      i, 0); }
  else if (eq("volatile"  , 8)) { return_token(T_VOLATILE,  i, 0); }
  else if (eq("while"     , 5)) { return_token(T_WHILE,     i, 0); }
  else if (eq("..."       , 3)) { return_token(T_VARIADIC,  i, 0); }
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

static token_t* find_definition(token_node_t* start, token_t* token) {
  for (; (NULL != start); start = start->prev) {
    /* TODO: Scope symbols with braces? */
    token_t* t = &start->token;
    if (T_IDENT != t->type) continue;
    if (t->len != token->len) continue;
    if ((t->datatype == DT_NONE) && (0 == t->function_prototype)) {
      if (NULL != t->definition) {
        if (0 == memcmp(t->definition->ptr, token->ptr, token->len)) {
          return t->definition;
        }
      }
      continue;
    }
    if (0 == memcmp(t->ptr, token->ptr, token->len)) return t;
  }
  return NULL;
}

int token_list_init(token_list_t* list, file_t* file) {
  token_t* previous_previous_token = NULL;
  token_t* previous_token = NULL;
  list->filename = file->filename;
  list->filesize = file->len;
  lexer_new(file->buf, file->buf + file->len);

  while (text_left(&lexer) > 1) {
    token_t token;

    token = lexer_next(&lexer);

    /* If the found token is an identifer, a lot of conclusions can be made */
    if (T_IDENT == token.type) {
      debug4("Found identifier: '%s' at line %u col %u offset %lu",
             token_name(&token), token.line, token.column, token.offset );
      /* Replicate datatypes with the first info found searching backwards
       * NOTE: This can have scope implications */
      if (NULL != list->last) {
        token.definition = find_definition(list->last, &token);
        if (NULL != token.definition) {
          if (token.definition->datatype) {
            token.datatype = token.definition->datatype;
            debug2("'%s' is a type, defined at line %u",
                   token_name(&token), token.definition->line);
          }
          else if (token.definition->function_prototype) {
            token.function_prototype = token.definition->function_prototype;
            debug2("'%s' is a function or functino call, defined at line %u",
                   token_name(&token), token.definition->line);
          }
        }
      }
    }

    /* Tag some keywords as datatypes as well */
    else if ((T_AUTO == token.type) ||
             (T_CHAR == token.type) ||
             (T_DOUBLE == token.type) ||
             (T_FLOAT == token.type) ||
             (T_INT == token.type) ||
             (T_LONG == token.type) ||
             (T_SHORT == token.type) ||
             (T_SIGNED == token.type) ||
             (T_UNSIGNED == token.type) ||
             (T_VOID == token.type)) {
      debug1("Setting '%s' as DT_PLAIN", token_name(&token));
      token.datatype = DT_PLAIN;
    }

    /* It's perfectly possible to figure out if we stubled upon a function
     * prototype */
    else if ((T_LPAREN == token.type) &&
             (NULL != previous_token) &&
             (NULL != previous_previous_token) &&
             (T_IDENT == previous_token->type)) {
      const int datatype = ((previous_previous_token->datatype) ||
                            (T_AUTO == previous_previous_token->type) ||
                            (T_CHAR == previous_previous_token->type) ||
                            (T_DOUBLE == previous_previous_token->type) ||
                            (T_FLOAT == previous_previous_token->type) ||
                            (T_INT == previous_previous_token->type) ||
                            (T_LONG == previous_previous_token->type) ||
                            (T_SHORT == previous_previous_token->type) ||
                            (T_SIGNED == previous_previous_token->type) ||
                            (T_UNSIGNED == previous_previous_token->type) ||
                            (T_VOID == previous_previous_token->type));
      debug2("Foo? %d %d", datatype, token.datatype);
      if ((datatype) ||
          (T_CONST == previous_previous_token->type) ||
          (T_STAR == previous_previous_token->type)) {
        previous_token->function_prototype = 1;
        debug1("Setting '%s' as function prototype",
               token_name(previous_token));
      }
    }

    /* This is border-line parsing, however - we're already doing smart things
     * so why not make things easier for the prototype.c code? */

    /* TODO: Deal with it?
     * - function pointers as typedefs
     * - arrays as typedefs
     */

    else if (T_TYPEDEF == token.type) { lexer.typedef_scan = 1; }
    else if (T_ENUM    == token.type) { lexer.enum_scan    = 1; }
    else if (T_UNION   == token.type) { lexer.union_scan   = 1; }
    else if (T_STRUCT  == token.type) { lexer.struct_scan  = 1; }

    else if ((NULL != previous_token) && (0 == lexer.brace_depth)) {
      if (T_IDENT != token.type) {
        /* If the syntax suggests anonymous enum, union or struct, terminate
         * scanning for these types */
        if (T_ENUM == previous_token->type)        { lexer.enum_scan   = 0; }
        else if (T_UNION == previous_token->type)  { lexer.union_scan  = 0; }
        else if (T_STRUCT == previous_token->type) { lexer.struct_scan = 0; }
      }
      if (T_IDENT == previous_token->type) {
        /* It seems the previous token was an identifier... */
        if (T_LBRACE == token.type) {
          if (lexer.enum_scan) {
            previous_token->datatype = DT_ENUM;
            debug1("Setting '%s' as enum type", token_name(previous_token));
            lexer.enum_scan = 0;
          }
          else if (lexer.union_scan) {
            previous_token->datatype = DT_UNION;
            debug1("Setting '%s' as union type", token_name(previous_token));
            lexer.union_scan = 0;
          }
          else if (lexer.struct_scan) {
            previous_token->datatype = DT_STRUCT;
            debug1("Setting '%s' as struct type", token_name(previous_token));
            lexer.struct_scan = 0;
          }
        }
        else if (T_SEMICOLON == token.type) {
          if (lexer.struct_scan) { /* Forward declaration */
            previous_token->datatype = DT_STRUCT;
            lexer.struct_scan = 0;
          }
          if (lexer.typedef_scan) {
            previous_token->datatype = DT_PLAIN;
            debug1("Setting '%s' as plan type", token_name(previous_token));
            /* Terminate nested scan (anonymous enum, struct and union) */
            lexer.enum_scan = lexer.union_scan = lexer.struct_scan = 0;
          }
        }
      }
    }
    /* Enable datatype scanners on level 0 */
    lexer.brace_depth += (T_LBRACE == token.type);
    lexer.brace_depth -= (T_RBRACE == token.type);
    previous_previous_token = previous_token;
    previous_token = token_list_append(list, &token);
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
  token_type_t waiting_for = 0;
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

static token_node_t* find_next_identifier(token_node_t* node) {
  for (; ((NULL != node) && (T_IDENT != node->token.type)); node = node->next);
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
