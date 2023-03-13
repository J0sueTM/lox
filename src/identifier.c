#include "identifier.h"

static int
lox_get_identifier_position
(
  char *_name
)
{
  if (_name == NULL)
  {
    fprintf(stderr, "given identifier name wasn't allocated\n");

    return -1;
  }

  int name_hash = 0;
  for (char *c = _name; *c != '\0'; ++c)
  {
    name_hash += (int)*c;
  }

  return name_hash % IDENTIFIER_TABLE_CAPACITY;
}

lox_identifier_table_t *
lox_create_and_populate_identifier_table
()
{
  // Using calloc since malloc creates garbage data on head_slots
  lox_identifier_table_t *new_table = calloc(1, sizeof(lox_identifier_table_t));
  if (new_table == NULL)
  {
    fprintf(stderr, "failed to allocate memory for new identifier table\n");

    return NULL;
  }
  new_table->identifier_count = 0;

  lox_push_identifier_to_table(new_table, "and", false, LOX_AND);
  lox_push_identifier_to_table(new_table, "class", false, LOX_CLASS);
  lox_push_identifier_to_table(new_table, "else", false, LOX_ELSE);
  lox_push_identifier_to_table(new_table, "false", false, LOX_FALSE);
  lox_push_identifier_to_table(new_table, "for", false, LOX_FOR);
  lox_push_identifier_to_table(new_table, "fun", false, LOX_FOR);
  lox_push_identifier_to_table(new_table, "if", false, LOX_FUN);
  lox_push_identifier_to_table(new_table, "nil", false, LOX_IF);
  lox_push_identifier_to_table(new_table, "or", false, LOX_NIL);
  lox_push_identifier_to_table(new_table, "print", false, LOX_OR);
  lox_push_identifier_to_table(new_table, "return", false, LOX_PRINT);
  lox_push_identifier_to_table(new_table, "super", false, LOX_RETURN);
  lox_push_identifier_to_table(new_table, "this", false, LOX_SUPER);
  lox_push_identifier_to_table(new_table, "true", false, LOX_THIS);
  lox_push_identifier_to_table(new_table, "var", false, LOX_TRUE);
  lox_push_identifier_to_table(new_table, "while", false, LOX_VAR);

  return new_table;
}

lox_identifier_t *
lox_push_identifier_to_table
(
  lox_identifier_table_t *_table,
  char                   *_name,
  bool                    _is_name_allocated,
  lox_token_e             _type
)
{
  if (_table == NULL)
  {
    fprintf(stderr, "given table wasn't allocated\n");

    return NULL;
  }

  lox_identifier_t *new_identifier = malloc(sizeof(lox_identifier_t));
  if (new_identifier == NULL)
  {
    fprintf(stderr, "failed to allocate memory for new identifier");

    return NULL;
  }
  new_identifier->name = _name;
  new_identifier->is_name_allocated = _is_name_allocated;
  new_identifier->type = _type;

  const int position = lox_get_identifier_position(_name);
  if (position < 0)
  {
    if (_is_name_allocated)
    {
      free(new_identifier->name);
    }

    free(new_identifier);

    return NULL;
  }

  lox_push_identifier_to_slot(_table, position, new_identifier);

  ++_table->identifier_count;
  return new_identifier;
}

void
lox_push_identifier_to_slot
(
  lox_identifier_table_t *_table,
  int                     _position,
  lox_identifier_t       *_identifier
)
{
  if (_identifier == NULL)
  {
    fprintf(stderr, "given head and/or identifier weren't allocated\n");

    return;
  }

  lox_identifier_t *last_identifier = NULL;
  lox_identifier_t *current_identifier = _table->head_slots[_position];
  while (current_identifier != NULL)
  {
    last_identifier = current_identifier;
    current_identifier = current_identifier->next;
  }

  if (last_identifier == NULL)
  {
    _table->head_slots[_position] = _identifier;
  }
  else
  {
    last_identifier->next = _identifier;
  }
}


lox_identifier_t *
lox_find_identifier
(
  lox_identifier_table_t *_table,
  char                   *_name
)
{
  if (_table == NULL)
  {
    fprintf(stderr, "given table wasn't allocated\n");

    return NULL;
  }

  const int position = lox_get_identifier_position(_name);
  if (position < 0)
  {
    return NULL;
  }

  lox_identifier_t *current_identifier = _table->head_slots[position];
  while (current_identifier != NULL)
  {
    if (current_identifier->name == _name)
    {
      break;
    }

    current_identifier = current_identifier->next;
  }

  return current_identifier;
}

void
lox_debug_identifier_table
(
  lox_identifier_table_t *_table
)
{
  if (_table == NULL)
  {
    fprintf(stderr, "given table wasn't allocated\n");

    return;
  }

  lox_identifier_t *current_head;
  for (int i = 0; i < IDENTIFIER_TABLE_CAPACITY; ++i)
  {
    current_head = _table->head_slots[i];
    lox_debug_subsequent_identifiers(current_head);
  }
}

void
lox_debug_subsequent_identifiers
(
  lox_identifier_t *_head
)
{
  if (_head == NULL)
  {
    return;
  }

  lox_identifier_t *current_identifier = _head;
  while (current_identifier != NULL)
  {
    printf("id [%d]: %s\n", current_identifier->type, current_identifier->name);
    current_identifier = current_identifier->next;
  }
}

void
lox_clean_identifier_table
(
  lox_identifier_table_t *_table
)
{
  if (_table == NULL)
  {
    fprintf(stderr, "given head identifier table wasn't allocated\n");

    return;
  }

  lox_identifier_t *current_head;
  int freed_identifier_count = 0;
  for (int i = 0; i < IDENTIFIER_TABLE_CAPACITY; ++i)
  {
    current_head = _table->head_slots[i];
    if (current_head != NULL)
    {
      freed_identifier_count += lox_clean_subsequent_identifiers(current_head);
    }
  }

  if (freed_identifier_count != _table->identifier_count)
  {
    fprintf(stderr, "possible memory leak: freed only %d ids out of %d\n",
            freed_identifier_count, _table->identifier_count);
  }

  free(_table);
}

int
lox_clean_subsequent_identifiers
(
  lox_identifier_t *_head
)
{
  if (_head == NULL)
  {
    fprintf(stderr, "given identifier head node wasn't allocated\n");

    return 0;
  }

  int freed_identifier_count = 0;
  lox_identifier_t *current_identifier = _head;
  lox_identifier_t *next_identifier = _head->next;
  while (current_identifier != NULL)
  {
    if (current_identifier->is_name_allocated)
    {
      free(current_identifier->name);
    }

    free(current_identifier);
    current_identifier = next_identifier;

    if (next_identifier != NULL)
    {
      next_identifier = next_identifier->next;
    }

    ++freed_identifier_count;
  }

  return freed_identifier_count;
}