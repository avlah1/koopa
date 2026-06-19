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
  char** args;
  int num_args;

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
    ParseResult token_res = TokenizeLine(line, &args, &num_args);
    if (token_res != PARSE_OK) {
      free(line);
      if (token_res == PARSE_SYSTEM_ERROR) {
        return false;
      }
      fprintf(stderr, ERROR " argument missing balanced quotes\n");
      continue;
    }
    Command* cmd;
    ParseResult parse_res = ParseLine(args, num_args, &cmd);
    if (parse_res != PARSE_OK) {
      free(line);
      free(args);
      if (parse_res == PARSE_SYSTEM_ERROR) {
        return false;
      }
      fprintf(stderr, ERROR" expected file name for i/o redirection\n");
      continue;
    }

    // EXECUTE COMMANDS
    ShellStatus status = Execute(cmd);
    free(line);
    free(args);
    free(cmd);
    if (status == SHELL_EXIT) {
      break;
    }
  }
  return true;
}

bool GetCurrentDir(char** path_ptr_ret) {
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
  *path_ptr_ret = dirname;
  return true;
}
