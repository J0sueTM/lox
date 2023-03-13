/*
 * Implements a hash set.
 */

#ifndef LOX_IDENTIFIER_H
#define LOX_IDENTIFIER_H

#include "base.h"

#define IDENTIFIER_TABLE_CAPACITY 16

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

typedef struct lox_identifier_t lox_identifier_t;
typedef struct lox_identifier_t
{
  char        *name;
  bool         is_name_allocated;
  lox_token_e  type;

  lox_identifier_t *next;
} lox_id_t;

typedef struct lox_identifier_table_t
{
  lox_identifier_t *head_slots[IDENTIFIER_TABLE_CAPACITY];
  int               identifier_count;
} lox_identifier_table_t;

lox_identifier_table_t *
lox_create_and_populate_identifier_table
();

lox_identifier_t *
lox_push_identifier_to_table
(
  lox_identifier_table_t *_table,
  char                   *_name,
  bool                    _is_name_allocated,
  lox_token_e             _type
);

void
lox_push_identifier_to_slot
(
  lox_identifier_table_t *_table,
  int                     _position,
  lox_identifier_t       *_identifier
);

void
lox_debug_identifier_table
(
  lox_identifier_table_t *_table
);

void
lox_debug_subsequent_identifiers
(
  lox_identifier_t *_head
);

void
lox_clean_identifier_table
(
  lox_identifier_table_t *_table
);

int
lox_clean_subsequent_identifiers
(
  lox_identifier_t *_head
);

#endif // LOX_IDENTIFIER_H
