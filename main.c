#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdbool.h>
#include <string.h>

#include "lexer.h"

FILE *parse_args(
  char *_file_name
);

char *read_file_content(
  FILE *_file
);

int main(
  int    _argc,
  char **_argv
)
{
  FILE *file = parse_args(_argv[1]);
  if (file == NULL)
  {
    return EXIT_FAILURE;
  }

  char *source = read_file_content(file);
  if (source == NULL)
  {
    return EXIT_FAILURE;
  }

  lox_token_t *head_token = lox_lexer_analyze_source(source);
  if (head_token == NULL)
  {
    return EXIT_FAILURE;
  }

  lox_lexer_debug_tokens(head_token);

  // lox_lexer_clean(head_token);

  return EXIT_SUCCESS;
}

FILE *parse_args(
  char *_file_name
)
{
  FILE *input_file = fopen(_file_name, "r");
  if (input_file == NULL)
  {
    fprintf(stderr, "failed to open source file\n");

    return NULL;
  }

  return input_file;
}

char *read_file_content(FILE *_file)
{
  long file_size;
  char *content_buffer;

  fseek(_file, 0L, SEEK_END);
  file_size = ftell(_file);
  fseek(_file, 0L, SEEK_SET);

  content_buffer = malloc(file_size * sizeof(char));
  if (content_buffer == NULL)
  {
    fprintf(stderr, "failed to allocate memory for file buffer");
    fclose(_file);

    return NULL;
  }

  fread(content_buffer, sizeof(char), file_size, _file);
  fclose(_file);

  return content_buffer;
}