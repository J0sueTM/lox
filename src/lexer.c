#include "lexer.h"

static bool
lox_lexer_verify_digit
(
  char _digit
)
{
  return (_digit >= '0' && _digit <= '9');
}

static bool
lox_lexer_verify_alpha
(
  char _char
)
{
  return (_char >= 'a' && _char <= 'z') ||
         (_char >= 'A' && _char <= 'Z') ||
         (_char == '_');
}

lox_token_t
*lox_push_token
(
  lox_lexer_t *_lexer,
  lox_token_t *_last_token,
  lox_token_e  _type,
  char        *_lexeme,
  bool         _is_lexeme_allocated,
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
  new_token->is_lexeme_allocated = _is_lexeme_allocated;
  new_token->literal = _literal;
  new_token->line = _lexer->line_count;
  new_token->next = NULL;

  if (_last_token != NULL)
    _last_token->next = new_token;

  ++_lexer->token_count;

  return new_token;
}

lox_lexer_t
*lox_lexer_analyze_source
(
  char *_source
)
{
  lox_lexer_t *new_lexer = malloc(sizeof(lox_lexer_t));
  if (new_lexer == NULL)
  {
    fprintf(stderr, "failed to allocate memory for new lexer\n");
    
    return NULL;
  }
  new_lexer->line_count = 0;

  new_lexer->identifier_table = lox_create_and_populate_identifier_table();
  if (new_lexer->identifier_table == NULL)
  {
    return NULL;
  }

  new_lexer->head = lox_push_token(new_lexer,
                                   NULL,
                                   LOX_BOF,
                                   "",
                                   false,
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
                 false,
                 NULL);

  return new_lexer;
}

lox_token_t
*lox_lexer_scan_tokens
(
  lox_lexer_t *_lexer
)
{
  lox_token_t *current_token = _lexer->head;
  long char_count = 0;
  char *current_char = _lexer->source;
  while (*current_char != '\0' && char_count < strlen(_lexer->source))
  {
    switch (*current_char)
    {
      case ' ':
      case '\r':
      case '\t':
      case '#':
        break;
      case '\n':
        ++_lexer->line_count;

        break;
      case '(':
        lox_push_token(_lexer, current_token, LOX_LEFT_PAREN,
                       "(", false, NULL);

        break;
      case ')':
        lox_push_token(_lexer, current_token, LOX_RIGHT_PAREN,
                       ")", false, NULL);

        break;
      case '{':
        lox_push_token(_lexer, current_token, LOX_LEFT_BRACE,
                       "{", false, NULL);

        break;
      case '}':
        lox_push_token(_lexer, current_token, LOX_RIGHT_BRACE,
                       "}", false, NULL);

        break;
      case ',':
        lox_push_token(_lexer, current_token, LOX_COMMA,
                       ",", false, NULL);

        break;
      case '.':
        lox_push_token(_lexer, current_token, LOX_DOT,
                       ".", false, NULL);

        break;
      case '-':
        lox_push_token(_lexer, current_token, LOX_MINUS,
                       "-", false, NULL);

        break;
      case '+':
        lox_push_token(_lexer, current_token, LOX_PLUS,
                       "+", false, NULL);

        break;
      case ';':
        lox_push_token(_lexer, current_token, LOX_SEMICOLON,
                       ";", false, NULL);

        break;
      case '*':
        lox_push_token(_lexer, current_token, LOX_STAR,
                       "*", false, NULL);

        break;
      case '/':
        lox_push_token(_lexer, current_token, LOX_SLASH,
                       "/", false, NULL);

        break;
      case '!':
      {
        int chars_to_skip = lox_lexer_scan_long_lexeme(_lexer, current_token, current_char,
                                                       "!=", LOX_BANG, LOX_BANG_EQUAL);
        char_count += chars_to_skip;
      } break;
      case '=':
      {
        int chars_to_skip = lox_lexer_scan_long_lexeme(_lexer, current_token, current_char,
                                                       "==", LOX_EQUAL, LOX_EQUAL_EQUAL);

        char_count += chars_to_skip;
      } break;
      case '<':
      {
        int chars_to_skip = lox_lexer_scan_long_lexeme(_lexer, current_token, current_char,
                                                       "<=", LOX_LESS, LOX_LESS_EQUAL);

        char_count += chars_to_skip;
      } break;
      case '>':
      {
        int chars_to_skip = lox_lexer_scan_long_lexeme(_lexer, current_token, current_char,
                                                       ">=", LOX_GREATER, LOX_GREATER_EQUAL);

        char_count += chars_to_skip;
      } break;
      case '"':
      {
        int chars_to_skip = lox_lexer_scan_string(_lexer, current_token, current_char);

        char_count += chars_to_skip;
      } break;
      default:
      {
        if (lox_lexer_verify_digit(*current_char))
        {
          int chars_to_skip = lox_lexer_scan_number(_lexer, current_token, current_char);

          char_count += chars_to_skip;
        }
        else if (lox_lexer_verify_alpha(*current_char))
        {
          int chars_to_skip = lox_lexer_scan_identifier(_lexer, current_token, current_char);

          char_count += chars_to_skip;
        }
        else
        {
          fprintf(stderr, "unexpected character %c\n", *current_char);
        }
      } break;
    }

    if (current_token->next != NULL)
    {
      current_token = current_token->next;
    }

    ++char_count;
    current_char = _lexer->source + char_count;
  }

  return current_token;
}

int
lox_lexer_scan_long_lexeme
(
  lox_lexer_t *_lexer,
  lox_token_t *_current_token,
  char        *_lexeme,
  char        *_compare,
  lox_token_e  _short_token,
  lox_token_e  _long_token
)
{
  bool has_compare = lox_lexer_look_ahead_and_match(_lexeme, _compare + 1);

  const unsigned long compare_length = strlen(_compare);
  char *lexeme = calloc(compare_length, sizeof(char));
  if (lexeme == NULL)
  {
    fprintf(stderr, "failed to allocate memory for %c lexeme\n", *_lexeme);

    return -1;
  }

  lox_token_e token_type = _long_token;
  int chars_to_skip = (int)(compare_length);
  if (has_compare)
  {
    sprintf(lexeme, "%s", _compare);
  }
  else
  {
    token_type = _short_token;
    *lexeme = *_compare;
    chars_to_skip = 0;
  }

  lox_push_token(_lexer, _current_token,
                 token_type, lexeme,
                 true, NULL);

  return chars_to_skip;
}

int
lox_lexer_scan_string
(
  lox_lexer_t *_lexer,
  lox_token_t *_current_token,
  char        *_lexeme
)
{
  if (*_lexeme != '"')
  {
    fprintf(stderr, "given lexeme isn't a string\n");

    return -1;
  }

  int string_length = 0;
  char *current_char = _lexeme + 1;
  bool should_continue_reading = true;
  while (should_continue_reading)
  {
    if (*current_char == '"' || *current_char == '\0')
    {
      should_continue_reading = false;

      continue;
    }

    ++string_length;
    ++current_char;
  }

  if (*current_char == '\0')
  {
    fprintf(stderr, "unterminated string\n");

    return -1;
  }

  char *string_buffer = calloc(string_length, sizeof(char));
  if (string_buffer == NULL)
  {
    fprintf(stderr, "failed to allocate memory while reading string\n");

    return -1;
  }
  strncpy(string_buffer, (_lexeme + 1), string_length);

  lox_push_token(_lexer, _current_token, LOX_STRING,
                 "string", false, (void *)string_buffer);

  return string_length + 1;
}

int
lox_lexer_scan_number
  (
    lox_lexer_t *_lexer,
    lox_token_t *_current_token,
    char        *_lexeme
)
{
  if (!lox_lexer_verify_digit(*_lexeme))
  {
    fprintf(stderr, "given lexeme isn't a number\n");

    return -1;
  }

  int number_length = 0;
  char *current_char = _lexeme;
  bool should_continue_reading = true;
  while (should_continue_reading)
  {
    if (!lox_lexer_verify_digit(*current_char))
    {
      if (*current_char == '.' && lox_lexer_verify_digit(*(current_char + 1)))
      {
        ++number_length;
        ++current_char;
      }
      else
      {
        should_continue_reading = false;

        continue;
      }
    }

    ++number_length;
    ++current_char;
  }

  char *number_buffer = calloc(number_length, sizeof(char));
  if (number_buffer == NULL)
  {
    fprintf(stderr, "failed to allocate memory for number lexeme\n");

    return -1;
  }
  strncpy(number_buffer, _lexeme, number_length);

  char *temp_buffer;
  double *parsed_number = malloc(sizeof(double));
  if (parsed_number == NULL)
  {
    fprintf(stderr, "failed to allocate memory for number\n");

    return -1;
  }
  *parsed_number = strtod(number_buffer, &temp_buffer);

  lox_push_token(_lexer, _current_token, LOX_NUMBER,
                 "number", false, (void *)parsed_number);

  free(number_buffer);

  return number_length;
}

int
lox_lexer_scan_identifier
(
  lox_lexer_t *_lexer,
  lox_token_t *_current_token,
  char        *_lexeme
)
{
  if (!lox_lexer_verify_alpha(*_lexeme))
  {
    fprintf(stderr, "given lexeme isn't alpha");

    return -1;
  }

  int identifier_length = 0;
  char *current_char = _lexeme;
  while (lox_lexer_verify_alpha(*current_char) ||
         lox_lexer_verify_digit(*current_char))
  {
    ++current_char;
    ++identifier_length;
  }

  char *identifier_name_buffer = calloc(identifier_length, sizeof(char));
  if (identifier_name_buffer == NULL)
  {
    fprintf(stderr, "failed to allocate memory for identifier buffer\n");

    return -1;
  }
  strncpy(identifier_name_buffer, _lexeme, identifier_length);
  
  lox_identifier_t *identifier = lox_find_identifier(_lexer->identifier_table, identifier_name_buffer);
  if (identifier == NULL)
  {
    lox_push_token(_lexer, _current_token, LOX_IDENTIFIER, "id", false, identifier_name_buffer);
  }
  else
  {
    lox_push_token(_lexer, _current_token, identifier->type, "id", false, identifier->name);

    free(identifier_name_buffer);
  }

  return identifier_length;
}

bool
lox_lexer_look_ahead_and_match(
  char *_lexeme,
  char *_compare
)
{
  unsigned long length_ahead = strlen(_compare);
  bool has_compare = strncmp((_lexeme + 1), _compare, length_ahead) == 0;

  return has_compare;
}

void
lox_lexer_debug_tokens
(
  lox_lexer_t *_lexer
)
{
  lox_token_t *current_token = _lexer->head;
  long token_count = 0;
  while (current_token != NULL && token_count < _lexer->token_count)
  {
    printf("tok [%p]: %s %p\n", current_token, current_token->lexeme,
           (current_token->literal != NULL) ? current_token->literal : NULL);

    current_token = current_token->next;
    ++token_count;
  }
}

void
lox_lexer_clean
(
  lox_lexer_t *_lexer
)
{
  lox_clean_identifier_table(_lexer->identifier_table);

  lox_token_t *current_token, *next_token;
  current_token = _lexer->head;
  next_token = current_token->next;
  while (current_token != NULL)
  {
    if (current_token->is_lexeme_allocated)
    {
      free(current_token->lexeme);
    }

    if (current_token->literal != NULL &&
        current_token->type == LOX_IDENTIFIER)
    {
      free(current_token->literal);
    }

    free(current_token);
    current_token = next_token;

    if (next_token != NULL)
    {
      next_token = next_token->next;
    }
  }
}