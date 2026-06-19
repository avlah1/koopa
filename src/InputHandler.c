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

/*

typedef struct {
  char** args;
  char* input_file;
  char* output_file;
  bool append;
  bool background;
} Command;
*/
ParseResult ParseLine(char** args, int num_args, Command** cmd_ret) {
  // what are the things that could go wrong??
  Command* cmd = (Command*) calloc(1, sizeof(Command));
  if (cmd == NULL) {
    perror("calloc failed in ParseLine");
    return PARSE_SYSTEM_ERROR;
  }
  for (int i = 0; i < num_args; i++) {
    if ((strcmp(args[i], ">") == 0) || (strcmp(args[i], ">>") == 0)) {
      if (i == num_args - 1) {
        free(cmd);
        return PARSE_BAD_INPUT;
      }
      cmd->output_file = args[i + 1];
      cmd->append = strcmp(args[i], ">>") == 0;
    } else if (strcmp(args[i], "<") == 0) {
      if (i == num_args - 1) {
        free(cmd);
        return PARSE_BAD_INPUT;
      }
      cmd->input_file = args[i + 1];
    }
  }
  cmd->args = args;
  cmd->num_args = num_args;
  *cmd_ret = cmd;
  return PARSE_OK;
}

ParseResult TokenizeLine(char* line, char*** args_ptr_ret, int* num_args_ret) {
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
        free(tokens);
        return PARSE_SYSTEM_ERROR;
      }
    }
    if (GetToken(NULL, &token, DELIMITERS) == PARSE_BAD_INPUT) {
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
  
  QuoteState state = QUOTE_NONE;
  if (*str == '"') {
    state = QUOTE_DOUBLE;
    str++;
  } else if (*str == '\'') {
    state = QUOTE_SINGLE;
    str++;
  }
  char* token_start = str;
  int position = 0;

  while (*str != '\0') {
    if (*str == '"') {
      if (state == QUOTE_NONE) {
        state = QUOTE_DOUBLE;
      } else if (state == QUOTE_DOUBLE) {
        state = QUOTE_NONE;
      } else {
        token_start[position] = *str;
        position++;
      }
      str++;
    } else if (*str == '\'') {
      if (state == QUOTE_NONE) {
        state = QUOTE_SINGLE;
      } else if (state == QUOTE_SINGLE) {
        state = QUOTE_NONE;
      } else {
        token_start[position] = *str;
        position++;
      }
      str++;
    } else if (state == QUOTE_NONE && strchr(delimiters, *str) != NULL) {
      break;
    } else {
      token_start[position] = *str;
      position++;
      str++;
    } 
  }
  if (state != QUOTE_NONE) {
    return PARSE_BAD_INPUT;
  }
  if (*str != '\0') {
    next_token_start = str + 1;
  } else {
    next_token_start = NULL;
  }
  token_start[position] = '\0';
  *token_ret = token_start;
  return PARSE_OK;
}