#ifndef LOX_LEXER_H
#define LOX_LEXER_H

#include "base.h"
#include "identifier.h"

typedef struct lox_token_t lox_token_t;
typedef struct lox_token_t
{
  lox_token_e  type;
  char        *lexeme;
  bool         is_lexeme_allocated;
  void        *literal;
  long         line;

  lox_token_t *next;
} lox_token_t;

typedef struct lox_lexer_t
{
  lox_token_t *head;

  lox_identifier_table_t *identifier_table;
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

int
lox_lexer_scan_identifier
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