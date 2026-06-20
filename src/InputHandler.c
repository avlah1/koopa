#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../include/InputHandler.h"

#define BUFSIZE 256
#define DELIMITERS " \n\t"

ReadResult ReadLine(char** line_ret) {
  size_t buffer_size = BUFSIZE;
  char* buffer = (char*) malloc(sizeof(char) * buffer_size);
  if (buffer == NULL) {
    perror("malloc failed in ReadLine");
    return READ_SYSTEM_ERROR;
  }
  int position = 0;
  int c;
  while ((c = getchar()) != EOF) {
    if (c == '\n') {
      buffer[position] = '\0';
      *line_ret = buffer;
      return READ_OK;
    } else {
      buffer[position] = c;
    }
    position++;
    if (position >= buffer_size) {
      buffer_size += BUFSIZE;
      buffer = realloc(buffer, buffer_size);
      if (buffer == NULL) {
        perror("realloc failed in ReadLine");
        free(buffer);
        return READ_SYSTEM_ERROR;
      }
    }
  }
  free(buffer);
  return READ_EOF;
}

static void ShiftArgs(char** args, int start, int end, int shift) {
  for (int i = start; i <= end; i++) {
    args[i - shift] = args[i];
  }
}

ParseResult ParseLine(char** args, int num_args, Command** cmd_ret) {
  Command* cmd = (Command*) calloc(1, sizeof(Command));
  if (cmd == NULL) {
    perror("calloc failed in ParseLine");
    return PARSE_SYSTEM_ERROR;
  }
  int i = 0;
  while (i < num_args) {
    if ((strcmp(args[i], ">") == 0) || (strcmp(args[i], ">>")) == 0) {
      if (i == num_args -1) {
        free(cmd);
        return PARSE_BAD_INPUT;
      }
      cmd->output_file = args[i + 1];
      cmd->append = strcmp(args[i], ">>") == 0;
      ShiftArgs(args, i + 2, num_args, 2);
      num_args -= 2;
    } else if (strcmp(args[i], "<") == 0) {
      if (i == num_args -1) {
        free(cmd);
        return PARSE_BAD_INPUT;
      }
      cmd->input_file = args[i + 1];
      ShiftArgs(args, i + 2, num_args, 2);
      num_args -= 2;
    } else {
      i++;
    }
  }
  cmd->args = args;
  cmd->num_args = num_args;
  *cmd_ret = cmd;
  return PARSE_OK;
}


// Static helper that splits the C-string "str" at delimeters "delimeters", one at a time. 
// Tokens are split with a '\0'.  If successful,
// a pointer to a newly formed token is returned via the return parameter "token_ret".
// A first call to this function should place the actual string to be tokenized at str.
// For subsequent calls, NULL should be passed as "str".
// This function returns:
// - PARSE_OK when the end of the string "str" is reached (eg, a null character)
// - PARSE_BAD_INPUT when a portion of "str" is unexpected (eg, unbalanced quotes)
static ParseResult GetToken(char* str, char** token_ret, char* delimiters) {
  static char* next_token_start = NULL;
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

  while (*str != '\0' && strchr(delimiters, *str) != NULL) { str++; }

  if (*str == '\0') {
    *token_ret = NULL;
    return PARSE_OK;
  }

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

