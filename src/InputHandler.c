#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../include/InputHandler.h"

#define BUFSIZE 256
#define DELIMITERS " \n\t"

bool ReadLine(char** line_ptr_ret) {
  size_t buffer_size = BUFSIZE;
  char* buffer = (char*) malloc(sizeof(char) * buffer_size);
  if (buffer == NULL) {
    perror("malloc failed in ReadLine");
    return false;
  }
  int position = 0;
  int c;
  while ((c = getchar()) != EOF) {
    if (c == '\n') {
      buffer[position] = '\0';
      *line_ptr_ret = buffer;
      return true;
    } else {
      buffer[position] = c;
    }
    position++;
    if (position >= buffer_size) {
      buffer_size += BUFSIZE;
      buffer = realloc(buffer, buffer_size);
      if (buffer == NULL) {
        perror("realloc failed in ReadLine");
        break;
      }
    }
  }
  free(buffer);
  return false;
}

ParseResult ParseLine(char* line, char*** args_ptr_ret, int* num_args_ret) {
  int buffer_size = BUFSIZE;
  char* token;
  char** tokens = (char**) malloc(sizeof(char*) * buffer_size);
  if (tokens == NULL) {
    perror("malloc failed in ParseLine");
    return PARSE_SYSTEM_ERROR;
  }
  int position = 0;
  if (GetToken(line, &token, DELIMITERS) == PARSE_BAD_INPUT) {
    printf("unbalanced quotes1\n");
    free(tokens);
    return PARSE_BAD_INPUT;
  }
  while (token != NULL) {
    tokens[position] = token;
    position++;
    if (position >= buffer_size) {
      buffer_size += BUFSIZE;
      tokens = (char**) realloc(tokens, sizeof(char*) * buffer_size);
      if (tokens == NULL) {
        perror("realloc failed in ParseLine");
        free(tokens);
        return PARSE_SYSTEM_ERROR;
      }
    }
    if (GetToken(NULL, &token, DELIMITERS) == PARSE_BAD_INPUT) {
      printf("unbalanced quotes2\n");
      free(tokens);
      return PARSE_BAD_INPUT;
    }
  }
  tokens[position] = NULL;
  *args_ptr_ret = tokens;
  *num_args_ret = position;

  return PARSE_OK;
}

ParseResult GetToken(char* str, char** token_ret, char* delimiters) {
  static char* next_token_start = NULL; // Keep track of where we are at in the string
  
  // Check if this is the first call to this function and adjust str accordingly
  if (str != NULL) {
    next_token_start = str;
  } else {
    str = next_token_start;
  }

  // Check if there is anything left to tokenize
  // If str is NULL at this point, that means that nts was null
  // which means it was set to NULL from previous call.
  if (str == NULL || *str == '\0') {
    *token_ret = NULL;
    return PARSE_OK;
  }

  // Eat leading delimeters
  while (*str != '\0' && strchr(delimiters, *str) != NULL) {
    str++;
  }

  // Check if there is anything left to parse
  if (*str == '\0') {
    *token_ret = NULL;
    return PARSE_OK;
  }
  
  // Create quoted flag and marker for token start, checking for leading quotation
  // and adjusting accordingly.
  bool quoted = false;
  if (*str == '"') {
    quoted = true;
    str++;
  }
  char* token_start = str;
  
  while (*str != '\0') {
    if (*str == '"') {
      if (quoted) {
        quoted = false;
        break;
      }
      quoted = true;
      str++;
    } else if (!quoted && strchr(delimiters, *str) != NULL) {
      break;
    } else {
      str++;
    }
  }
 
  // Check for unbalanced quotes, this is considered a hard fail
  if (quoted) {
    return PARSE_BAD_INPUT;
  }
  
  // Check the current character. If it isnt a null character, 
  // then set it to null character and set nts to one position ahead of us
  // for subseqent call. 
  // Otherwise, we are at the end of the string. Set nts to null.
  // In either case, set the return parameter to token start and return true.
  if (*str != '\0') {
    *str = '\0';
    next_token_start = str + 1;
  } else {
    next_token_start = NULL;
  }
  *token_ret = token_start;
  return PARSE_OK;
 
}