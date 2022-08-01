typedef enum {
  /* Represents the absense of token. */
  T_NONE      = 0,

  /* Keywords */
  T_AUTO      = 201,
  T_BREAK,
  T_CASE,
  T_CHAR,
  T_CONST,
  T_CONTINUE,
  T_DEFAULT,
  T_DO,
  T_DOUBLE,
  T_ELSE,
  T_ENUM,
  T_EXTERN,
  T_FLOAT,
  T_FOR,
  T_GOTO,
  T_IF,
  T_INT,
  T_LONG,
  T_REGISTER,
  T_RETURN,
  T_SHORT,
  T_SIGNED,
  T_SIZEOF,
  T_STATIC,
  T_STRUCT,
  T_SWITCH,
  T_TYPEDEF,
  T_UNION,
  T_UNSIGNED,
  T_VOID,
  T_VOLATILE,
  T_WHILE,

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
  T_FLOAT     = 191, /* a floating point number */
  T_STRING    = 192, /* a string constant */
  T_CHAR      = 193, /* a character */

  T_IDENT     = 200, /* an identifier */

  T_EOF       = 299, /* End of file, no more tokens to parse */
} token_type_t;

/* Some tokens hold additional values, e.g., an integer token
 * has an integer value and an identifier token has a name. */
typedef union {
    ident_t ident;
    uint64_t i;
    double d;
    const char* s;
} token_value_t;

/* The complete representation of a token. */
typedef struct {
    token_value_t value;
    token_type_t type;
    int line;
    int column;
} token_t;
