#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../include/InputHandler.h"

#define BUFSIZE 256
#define DELIMITERS " \n\t"
#define DEBUG 0

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

// Maps a token string to its corresponding CondOpInfo enum value.
// Returns OP_NONE if the token is not a conditional operator.
static CondOpInfo GetCondOp(char* token) {
  if (strcmp(token, "&&") == 0) {
    return OP_AND;
  } else if (strcmp(token, "||") == 0) {
    return OP_OR;
  } else if (strcmp(token, ";") == 0) {
    return OP_SEP;
  }
  return OP_NONE;
}

// Parses a single command from a token range into a heap-allocated Command struct.
// Also handles an optional leading condtional operator (&& || ;), I/O operators
// (<, >, >>), and compies remaining tokens into cmd->args via strdup.
// The caller owns the returned Command and is responsible for freeing it.
static ParseResult ParseCommand(char** tokens, int num_tokens, Command** cmd_ret) {
  Command* cmd = (Command*) calloc(1, sizeof(Command));
  if (cmd == NULL) {
    perror("calloc failed in ParseCommand");
    return PARSE_SYSTEM_ERROR;
  }
  int buffer_size = BUFSIZE;
  char** args = (char**) malloc(sizeof(char*) * buffer_size);
  if (args == NULL) {
    CommandChain_FreeCommand(cmd);
    perror("malloc failed in ParseCommand");
    return PARSE_SYSTEM_ERROR;
  }
  int i = 0, position = 0;
  // Check if the first token is a conditional operator. If it is, move i forward by one
  // so we don't copy this into the args list for the command.
  CondOpInfo op = GetCondOp(tokens[0]);
  if (op != OP_NONE) {
    if (num_tokens == 1) {
      CommandChain_FreeCommand(cmd);
      fprintf(stderr, "kpa: expected additional argument for conditional: %s\n", tokens[i]);
      return PARSE_BAD_INPUT;
    }
    i++;
  }
  cmd->cond_op = op;
   while (i < num_tokens) {
    // I/O redirection
    if ((strcmp(tokens[i], ">") == 0) || (strcmp(tokens[i], ">>")) == 0) {
      if (i == num_tokens - 1) {
        fprintf(stderr, "kpa: expected filename for i/o redirection\n");
        CommandChain_FreeCommand(cmd);
        return PARSE_BAD_INPUT;
      }
      cmd->output_file = strdup(tokens[i + 1]);
      cmd->append = strcmp(tokens[i], ">>") == 0;
      i += 2;
    } else if (strcmp(tokens[i], "<") == 0) {
      if (i == num_tokens - 1) {
        CommandChain_FreeCommand(cmd);
        return PARSE_BAD_INPUT;
      }
      cmd->input_file = strdup(tokens[i + 1]);
      // Moving i forward by 2 allows for us to skip adding both the operator
      // and the file name to the args list for the command.
      i += 2;
    } else if (strcmp(tokens[i], "|") == 0) {
      cmd->pipe_next = true;
      i++;
    } else {
      args[position] = strdup(tokens[i]);
      cmd->num_args++;
      i++;
      position++;
      if (position >= buffer_size) {
        buffer_size += BUFSIZE;
        args = realloc(args, buffer_size);
        if (args == NULL) {
          perror("realloc failed in ParseCommand");
          free(args);
          CommandChain_FreeCommand(cmd);
          return PARSE_SYSTEM_ERROR;
        }
      }
    }    
  }
  // Null terminate, assign args, and return
  args[cmd->num_args] = NULL;
  cmd->args = args;
  *cmd_ret = cmd;
  return PARSE_OK;
}

ParseResult ParseCommandChain(char** tokens, int num_tokens, CommandChain** chain_ret) {
  CommandChain* chain;
  if (!CommandChain_Allocate(&chain)) {
    return PARSE_SYSTEM_ERROR;
  }
  Command* cmd;
  ParseResult result;
  int i = 0, start = 0;
  while (i < num_tokens) {
    bool pipe = strcmp(tokens[i], "|") == 0;
    if ((strcmp(tokens[i], "&&") == 0) ||
        (strcmp(tokens[i], "||") == 0) ||
        (strcmp(tokens[i], ";") == 0) || pipe) {
          // If the current token is a conditional operator, exclude it from this command.
          // Otherwise, it is a pipe token, so include it with this command.
          if (pipe && i == num_tokens - 1) {
            fprintf(stderr, "kpa: expected additional args near \"|\"\n");
            CommandChain_Free(chain);
            return PARSE_BAD_INPUT;
          }
          int range = pipe ? i - start + 1 : i - start;
          result = ParseCommand(&tokens[start], range, &cmd);
          if (result != PARSE_OK) {
            CommandChain_Free(chain);
            return result;
          }
          CommandChain_Append(chain, cmd);
          // If the final token was a pipe, exclude it from the next command. 
          // Otherwise, the final token is a conditional operator that needs
          // to be included with the next command.
          start = pipe ? i + 1 : i;  
    }
    i++;
  }
  // Parse the command that is terminated by NULL
  result = ParseCommand(&tokens[start], num_tokens - start, &cmd);
  if (result != PARSE_OK) {
    CommandChain_Free(chain);
    return result;
  }
  CommandChain_Append(chain, cmd);
  *chain_ret = chain;
  return PARSE_OK;
}

// Tokenizes a string in-place using strtok-style calling convention: pass the 
// string to tokenize on the first call, then NULL on subsequent calls to get remaining
// tokens. Handles single and double quotes: quoted sections suppress delimiter 
// splitting and strip quote characters. Single and double quotes are tracked independently.
// Returns PARSE_BAD_INPUT if unbalanced quotes are detected.
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
    fprintf(stderr, "kpa: argument missing balanced quotes\n");
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

