#ifndef LOX_LEXER_H
#define LOX_LEXER_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef enum lox_token_e
{
  // Single-character tokens.
  LOX_LEFT_PAREN, LOX_RIGHT_PAREN, LOX_LEFT_BRACE, LOX_RIGHT_BRACE,
  LOX_COMMA, LOX_DOT, LOX_MINUS, LOX_PLUS, LOX_SEMICOLON, LOX_SLASH, LOX_STAR,

  // One or two character tokens.
  LOX_BANG, LOX_BANG_EQUAL,
  LOX_EQUAL, LOX_EQUAL_EQUAL,
  LOX_GREATER, LOX_GREATER_EQUAL,
  LOX_LESS, LOX_LESS_EQUAL,

  // Literals.
  LOX_IDENTIFIER, LOX_STRING, LOX_NUMBER,

  // Keywords.
  LOX_AND, LOX_CLASS, LOX_ELSE, LOX_FALSE, LOX_FUN, LOX_FOR, LOX_IF, LOX_NIL, LOX_OR,
  LOX_PRINT, LOX_RETURN, LOX_SUPER, LOX_THIS, LOX_TRUE, LOX_VAR, LOX_WHILE,

  LOX_BOF, LOX_EOF
} lox_token_e;

typedef struct lox_token_t lox_token_t;
typedef struct lox_token_t
{
  lox_token_e  type;
  char    *lexeme;
  bool     is_lexeme_allocated;
  void    *literal;
  long     line;

  lox_token_t *next;
} lox_token_t;

typedef struct lox_lexer_t
{
  lox_token_t *head;

  char *source;

  long token_count;
  long line_count;
} lox_lexer_t;

lox_token_t
*lox_push_token
(
  lox_lexer_t *_lexer,
  lox_token_t *_last_token,
  lox_token_e  _type,
  char        *_lexeme,
  bool         _is_lexeme_allocated,
  void        *_literal
);

lox_lexer_t
*lox_lexer_analyze_source
(
  char *_source
);

lox_token_t
*lox_lexer_scan_tokens
(
  lox_lexer_t *_lexer
);

int
lox_lexer_scan_long_lexeme
(
  lox_lexer_t *_lexer,
  lox_token_t *_current_token,
  char        *_lexeme,
  char        *_compare,
  lox_token_e  _short_token,
  lox_token_e  _long_token
);

int
lox_lexer_scan_string
(
  lox_lexer_t *_lexer,
  lox_token_t *_current_token,
  char        *_lexeme
);

int
lox_lexer_scan_number
(
  lox_lexer_t *_lexer,
  lox_token_t *_current_token,
  char        *_lexeme
);

bool
lox_lexer_look_ahead_and_match
(
  char *_lexeme,
  char *_compare
);

void
lox_lexer_debug_tokens
(
  lox_lexer_t *_lexer
);

void
lox_lexer_clean
(
  lox_lexer_t *_lexer
);

#endif // LOX_LEXER_H