#ifndef _TOKENIZER_H_
#define _TOKENIZER_H_

#include "file.h"
#include "list.h"
#include "node.h"

#include <stdlib.h>


enum token_type_e {
  /* Represents the absense of token. */
  T_NONE      = 0,

  /* Keywords */
  T_AUTO      = 201,
  T_ASM,
  T_BREAK,
  T_CASE,
  T_CATCH,
  T_CHAR,
  T_CLASS,
  T_CONST,
  T_CONTINUE,
  T_DEFAULT,
  T_DELETE,
  T_DOUBLE,
  T_DO,
  T_ELSE,
  T_ENUM,
  T_EXTERN,
  T_FLOAT,
  T_FOR,
  T_FRIEND,
  T_GOTO,
  T_IF,
  T_INLINE,
  T___INLINE, /* Older compilers */
  T_INT,
  T_LONG,
  T_NEW,
  T_OPERATOR,
  T_PRIVATE,
  T_PROTECTED,
  T_PUBLIC,
  T_REGISTER,
  T_RETURN,
  T_SHORT,
  T_SIGNED,
  T_SIZEOF,
  T_STATIC,
  T_STRUCT,
  T_SWITCH,
  T_TEMPLATE,
  T_THIS,
  T_THROW,
  T_TRY,
  T_TYPEDEF,
  T_UNION,
  T_UNSIGNED,
  T_VIRTUAL,
  T_VOID,
  T_VOLATILE,
  T_WHILE,
  T_VARIADIC,

  /* Symbols */
  T_EXCLAM    = '!',
  T_LPAREN    = '(',
  T_RPAREN    = ')',
  T_LBRACE    = '{',
  T_RBRACE    = '}',
  T_LBRACKET  = '[',
  T_RBRACKET  = ']',
  T_MINUS     = '-',
  T_PLUS      = '+',
  T_STAR      = '*',
  T_SLASH     = '/',
  T_PERCENT   = '%',
  T_AMPER     = '&',
  T_PIPE      = '|',
  T_HAT       = '^',
  T_DOT       = '.',
  T_EQ        = '=',
  T_LE        = 68,  /* <= */
  T_LT        = '<',
  T_GT        = '>',
  T_GE        = 71,  /* >= */
  T_SEMICOLON = ';',
  T_COLON     = ':',
  T_COMMA     = ',',
  T_TILDE     = '~',
  T_QUESTION  = '?',
  T_CPP       = '#',

  T_PIPEEQ    = 300 + '|', // |=
  T_AMPEREQ   = 300 + '&', // &=
  T_PLUSEQ    = 300 + '+', // +=
  T_MINUSEQ   = 300 + '-', // -=
  T_SLASHEQ   = 300 + '/', // /=
  T_PERCENTEQ = 300 + '%', // %=
  T_STAREQ    = 300 + '*', // *=
  T_HATEQ     = 300 + '^', // ^=
  T_NE        = 300 + '!', // !=
  T_EQ2       = 300 + '=', // ==

  T_PLUS2     = 400 + '+', // ++
  T_MINUS2    = 400 + '-', // --
  T_LT2       = 400 + '<', // <<
  T_GT2       = 400 + '>', // >>

  /* Literals */
  T_INTEGER   = 190, /* an integer number */
  /*T_FLOAT     = 191,*/ /* a floating point number */
  T_STRING    = 192, /* a string constant */
  /*T_CHAR      = 193,*/ /* a character */

  T_IDENT     = 200, /* an identifier */

  T_EOF       = 299, /* End of file, no more tokens to parse */
};
typedef enum token_type_e token_type_t;


typedef unsigned int ident_t;

/* Some tokens hold additional values, e.g., an integer token
 * has an integer value and an identifier token has a name. */
typedef union {
  ident_t ident;
  unsigned long int i;
  double d;
  const char* s;
} token_value_t;

enum token_datatype_e {
  DT_NONE = 0,
  DT_PLAIN,
  DT_ENUM,
  DT_UNION,
  DT_STRUCT,
};
typedef enum token_datatype_e token_datatype_t;

struct token_s; /* Forward declaration */

/*
 * Token nodes and list
 */
struct token_usage_node_s {
  NODE_STRUCT(struct token_usage_node_s);
  struct token_s* token;
};
typedef struct token_usage_node_s token_usage_node_t;

struct token_usage_list_s {
  LIST_STRUCT(token_usage_node_t);
};
typedef struct token_usage_list_s token_usage_list_t;


/*
 * Token as datatype nodes and list
 */
struct token_type_node_s {
  NODE_STRUCT(struct token_type_node_s);
  struct token_s* token;
};
typedef struct token_type_node_s token_type_node_t;

struct token_type_list_s {
  LIST_STRUCT(token_type_node_t);
};
typedef struct token_type_list_s token_type_list_t;

enum argument_type_e {
  AT_PLAIN,
  AT_ARRAY,
  AT_FUNCTION_POINTER
};
typedef enum argument_type_e argument_type_t;

/*
 * Argument list nodes and list
 */
struct token_argument_node_s {
  NODE_STRUCT(struct token_argument_node_s);
  token_type_list_t type_list;
  argument_type_t type;
  struct token_s* token; /* Name of the argument (if any) */
};
typedef struct token_argument_node_s token_argument_node_t;

struct token_argument_list_s {
  LIST_STRUCT(token_argument_node_t);
};
typedef struct token_argument_list_s token_argument_list_t;

/*
 * Files (defined by # in a pre-processed source)
 */
struct token_file_s {
  char* name;
  int len;
  size_t current_line;
};
typedef struct token_file_s token_file_t;

struct token_file_node_s {
  NODE_STRUCT(struct token_file_node_s);
  token_file_t file;
};

typedef struct token_file_node_s token_file_node_t;

struct token_file_list_s {
  LIST_STRUCT(token_file_node_t);
  token_file_t* current;
};
typedef struct token_file_list_s token_file_list_t;

#define TOKEN_FILE_LIST_EMPTY LIST_INIT, NULL

/* The complete representation of a token. */
typedef struct token_s {
  int len;
  char* ptr;
  token_type_t type;
  size_t offset;
  unsigned int line;
  unsigned int column;
  token_datatype_t datatype;
  int function_prototype;
  struct token_s* definition;
  int used;
  token_usage_list_t usage_list;
  token_type_list_t return_type_list;
  token_argument_list_t argument_list;
  token_file_t* file;
  size_t file_line;
} token_t;

/* The context needed to tokenize code. */
typedef struct {
  /* "Coordinates" in the file */
  unsigned int line;
  unsigned int column;
  size_t offset;
  /* The remaining range of text to be tokenized. */
  const char* text_start;
  const char* text_next;
  const char* text_end;
  const char* text;
  /* Some initial parsing can efficiently be done in the lexer... */
  int typedef_scan;
  int enum_scan;
  int union_scan;
  int struct_scan;
  int attribute_scan;
  int file_scan;
  int brace_depth;
  int paren_depth;
  token_file_list_t file_list;
} lexer_t;

#define LEXER_EMPTY \
  1,                                            \
    1,                                          \
    0,                                          \
    NULL,                                       \
    NULL,                                       \
    NULL,                                       \
    NULL,                                       \
    0,                                          \
    0,                                          \
    0,                                          \
    0,                                          \
    0,                                          \
    0,                                          \
    0,                                          \
    0,                                          \
  {TOKEN_FILE_LIST_EMPTY}                       \


struct token_node_s {
  NODE_STRUCT(struct token_node_s);
  token_t token;
};
typedef struct token_node_s token_node_t;

struct token_list_s {
  LIST_STRUCT(token_node_t);
  const char* filename;
  size_t filesize;
  int brace_depth;
  token_node_t* current;
};
typedef struct token_list_s token_list_t;

#define TOKEN_LIST_EMPTY {NULL, 0, 0, NULL, NULL, 0}

char* token_name(const token_t* token);
int token_list_init(token_list_t* list, const file_t* file);
void token_list_cleanup(token_list_t* list);
token_node_t* token_list_find_function_declaration(token_node_t* node);
token_node_t* token_list_find_next_symbol_usage(token_list_t* list,
                                                token_node_t* node);
const
token_node_t* token_list_find_beginning_of_statement(const token_node_t* node);
const
token_node_t* token_list_find_end_of_argument_list(const token_node_t* node);

#endif
