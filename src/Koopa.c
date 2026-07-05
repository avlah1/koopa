#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "../include/Koopa.h"

#define BUFSIZE 256

#define DEBUG 1

int main(int argc, char** argv) {
  if (!ShellLoop()) {
    printf("Koopa exited with failure\n");
    return EXIT_FAILURE;
  }
  printf("Koopa exited with success\n");
  return EXIT_SUCCESS;
}

bool ShellLoop() {
  char* curr_dir;
  char* line;
  char** tokens;
  int num_tokens;

  while (true) {
    // DISPLAY
    printf(COLOR_GREEN"kpa:"COLOR_END);
    if (!GetCurrentDir(&curr_dir)) {
      return false;
    }
    printf(COLOR_BLUE"%s>> "COLOR_END, curr_dir);
    free(curr_dir);
    
    // READ
    ReadResult read_res = ReadLine(&line);
    if (read_res != READ_OK) {
      if (read_res == READ_EOF) {
        return true;
      }
      return false;
    }

    // TOKENIZE AND PARSE
    ParseResult token_res = TokenizeLine(line, &tokens, &num_tokens);
    if (token_res != PARSE_OK) {
      free(line);
      if (token_res == PARSE_SYSTEM_ERROR) {
        return false;
      }
      fprintf(stderr, ERROR " argument missing balanced quotes\n");
      continue;
    }
    // we will call ParseCommandChain
      // can return PARSE_SYSTEM FAILURE PARSEOK OR PARSE_BADINPUT
    CommandChain* chain;
    ParseResult parse_res = ParseCommandChain(tokens, num_tokens, &chain);
    if (parse_res != PARSE_OK) {
      free(line);
      free(tokens);
      if (parse_res == PARSE_SYSTEM_ERROR) {
        return false;
      }
      fprintf(stderr, COLOR_RED" expected filename for i/o redirection\n");
      continue;
    }
    free(line);
    free(tokens);
    ShellStatus status = Execute(chain);
    CommandChain_Free(chain);
    if (status == SHELL_EXIT) {
      break;
    }
  }
  return true;
}

bool GetCurrentDir(char** path_ret) {
  size_t buffer_size = BUFSIZE;
  char* ptr = NULL;
  char* dirname;
  do {
    dirname = (char*) malloc(sizeof(char) * buffer_size);
    if (dirname == NULL) {
      perror("malloc failed in GetCurrentDir");
      return false;
    }
    ptr = getcwd(dirname, buffer_size);
    if (ptr == NULL) {
      free(dirname);
      if (errno == ERANGE) {
        buffer_size *= 2;
      } else {
        perror("getcwd failed in GetCurrentDir");
        return false;
      }
    }
  } while (ptr == NULL);
  *path_ret = dirname;
  return true;
}
