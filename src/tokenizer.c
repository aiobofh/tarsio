#define return_token(type, dst, value)                           \
  do {                                                           \
    token_value_t __value;                                       \
    __value.dst = value;                                         \
    token_t __tok = {__value, type, lexer->line, lexer->column}; \
    return __tok;                                                \
  } while (0)

lexer_t lexer_new(const char* begin, const char* end) {
  lexer_t lexer;
  lexer.line = 1;
  lexer.column = 1;
  lexer.text_next = begin;
  lexer.text_end = end;
  return lexer;
}


static size_t text_left(lexer_t* lexer) {
  return lexer->text_end - lexer->text_next;
}

static token_t lex_number_hex(lexer_t* lexer) {
  lexer->text_next += 2;
  uint64_t value = 0;
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
  uint64_t value = 0;
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
  lexer->text_next += 2;
  uint64_t value = 0;
  while (1) {
    char d = *lexer->text_next;
    if ((d >= '0') && (d <= '7')) value = value * 8 + (d - '0');
    else if (d != '_') break;
    lexer->text_next++;
  }
  return_token(T_INTEGER, i, value);
}


static token_t lex_number_dec(lexer_t* lexer) {
  uint64_t value = 0;
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

static token_t lex_identifier(lexer_t* lexer) {
  const char* begin = lexer->text_next;
  ident_hash_t hash = IDENT_HASH_INIT;

  while (isalnum(*lexer->text_next) || (*lexer->text_next == '_')) {
    hash = ident_hashstep(*lexer->text_next, hash);
    lexer->text_next++;
  }

#define streq(length, string) \
  (end - begin == length) && (memcmp(begin, string, end - begin) == 0)

  const char* end = lexer->text_next;
  if      (streq(4, "auto"))     { return_token(T_AUTO, i ,0); }
  else if (streq(6, "struct"))   { return_token(T_STRUCT, i, 0); }
  else if (streq(5, "union"))    { return_token(T_UNION, i, 0); }
  else if (streq(4, "enum"))     { return_token(T_ENUM, i, 0); }
  else if (streq(3, "for"))      { return_token(T_FOR, i, 0); }
  else if (streq(5, "while"))    { return_token(T_WHILE, i, 0); }
  else if (streq(2, "do"))       { return_token(T_DO, i, 0); }
  else if (streq(2, "if"))       { return_token(T_IF, i, 0); }
  else if (streq(4, "else"))     { return_token(T_ELSE, i, 0); }
  else if (streq(3, "asm"))      { return_token(T_ASM, i, 0); }
  else if (streq(6, "switch"))   { return_token(T_SWITCH, i, 0); }
  else if (streq(4, "case"))     { return_token(T_CASE, i, 0); }
  else if (streq(5, "break"))    { return_token(T_BREAK, i, 0); }
  else if (streq(8, "continue")) { return_token(T_CONTINUE, i, 0); }
  else if (streq(5, "const"))    { return_token(T_CONST, i, 0); }
  else if (streq(8, "volatile")) { return_token(T_VOLATILE, i, 0); }
  else if (streq(6, "return"))   { return_token(T_RETURN, i, 0); }
  else if (streq(4, "cast"))     { return_token(T_CAST, i, 0); }
  else {  return_token(T_IDENT, ident, ident_intern(begin, end, hash)); }
#undef streq
}

static void skip_whitespace(lexer_t* lexer) {
  while (1) {
    // Skip space, tabs, newlines, etc
    for (; ((text_left(lexer) > 0) && isspace(lexer->text_next[0]));
         lexer->text_next++) {}

    char c = *lexer->text_next;
    char d = text_left(lexer) >= 1 ? lexer->text_next[1] : 0;

    // Skip line comments, i.e., "// this is a comment"
    if ((c == '/') && (d == '/')) {
      for (; (text_left(lexer) > 0) && (lexer->text_next[0] != '\n');
           lexer->text_next++) {}
      continue;

      // Skip block comments, i.e., "/* this is a comment */"
      // Note that block comments can be nested.
    } else if ((c == '/') && (d == '*')) {
      int nest_count = 1;
      lexer->text_next += 2;
      while ((nest_count > 0) && text_left(lexer) > 2) {
        int is_endblock = (lexer->text_next[0] == '*') && (lexer->text_next[1] == '/');
        int is_beginblock = (lexer->text_next[0] == '/') && (lexer->text_next[1] == '*');
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

token_t lexer_next(lexer_t* lexer) {
  skip_whitespace(lexer);

  char c = *lexer->text_next;
  char d = lexer->text_next < lexer->text_end ? *(lexer->text_next + 1) : 0;
  if (isdigit(c)) {
    return lex_number(lexer);
  } else if (isalpha(c) || (c == '_')) {
    return lex_identifier(lexer);

    // equals-kind tokens, e.g., ==, !=, +=, &=, etc
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
    return_token(300 + c, i, 0); // Check the definition for these tokens.

    // ++ -- << >>
  } else if ((d == c) && ((c == '+') ||
                          (c == '-') ||
                          (c == '<') ||
                          (c == '>'))) {
    lexer->text_next++;
    return_token(400 + c, i, 0); // Check the definition for these tokens.

    // Single character tokens
  } else if ((c == '(') || (c == ')') || (c == '{') || (c == '}') ||
             (c == '[') || (c == ']') || (c == '.') || (c == ';') ||
             (c == ':') || (c == ',') || (c == '~') || (c == '|') ||
             (c == '&') || (c == '+') || (c == '-') || (c == '+') ||
             (c == '*') || (c == '/') || (c == '%') || (c == '=') ||
             (c == '<') || (c == '>')) {
    lexer->text_next++;
    return_token(c, i, 0);
  }
  return_token(T_NONE, i, 0);
}
