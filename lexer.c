#include "lexer.h"

lox_token_t *lox_push_token(
  lox_lexer_t *_lexer,
  lox_token_t *_last_token,
  lox_token_e  _type,
  char        *_lexeme,
  void        *_literal
)
{
  lox_token_t *new_token = malloc(sizeof(lox_token_t));
  if (new_token == NULL)
  {
    fprintf(stderr, "failed to allocate memory for new token\n");

    return NULL;
  }

  new_token->type = _type;
  new_token->lexeme = _lexeme;
  new_token->literal = _literal;
  new_token->line = _lexer->line_count;
  new_token->next = NULL;

  if (_last_token != NULL)
    _last_token->next = new_token;

  ++_lexer->token_count;

  return new_token;
}

lox_lexer_t *lox_lexer_analyze_source(
  char *_source
)
{
  lox_lexer_t *new_lexer = malloc(sizeof(lox_lexer_t));
  if (new_lexer == NULL)
  {
    fprintf(stderr, "failed to allocate memory for new lexer\n");
    
    return NULL;
  }

  new_lexer->head = lox_push_token(new_lexer,
                                   NULL,
                                   LOX_BOF,
                                   "",
                                   NULL);
  if (new_lexer->head == NULL)
  {
    return NULL;
  }

  new_lexer->source = _source;
  lox_token_t *last_token = lox_lexer_scan_tokens(new_lexer);

  lox_push_token(new_lexer,
                 last_token,
                 LOX_EOF,
                 "",
                 NULL);

  return new_lexer;
}

lox_token_t *lox_lexer_scan_tokens(
  lox_lexer_t *_lexer
)
{
  lox_token_t *current_token = _lexer->head;
  long char_count = 0;
  char *current_char = _lexer->source;
  while (char_count < strlen(_lexer->source))
  {
    switch (*current_char)
    {
      case '(': {
        lox_token_t *new_token = lox_push_token(_lexer,
                                                current_token,
                                                LOX_LEFT_PAREN,
                                                "(",
                                                NULL);
      } break;
    }

    if (current_token->next != NULL)
      current_token = current_token->next;
    ++current_char;
    ++char_count;
  }

  return current_token;
}

void lox_lexer_debug_tokens(
  lox_lexer_t *_lexer
)
{
  lox_token_t *current_token = _lexer->head;
  long token_count = 0;
  while (current_token != NULL && token_count < _lexer->token_count)
  {
    printf("\n[%lu]: %s", token_count, current_token->lexeme);

    current_token = current_token->next;
    ++token_count;
  }
}

void lox_lexer_clean(
  lox_lexer_t *_lexer
)
{
  lox_token_t *current_token, *next_token;
  current_token = _lexer->head;
  next_token = current_token->next;

  while (current_token != NULL)
  {
    free(current_token);
    current_token = next_token;

    if (next_token->next != NULL)
    {
      next_token = next_token->next;
    }
  }
}