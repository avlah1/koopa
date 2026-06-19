#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "../include/Koopa.h"
#include "../include/InputHandler.h"
#include "../include/Execute.h"
#include "../include/Colors.h"

#define BUFSIZE 256

#define DEBUG 0

int main(int argc, char** argv) {
  if (!ShellLoop()) {
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

bool ShellLoop() {
  char* curr_dir;
  char* line;
  char** args;
  int num_args;
  bool status = true; 
  do {
    printf(COLOR_GREEN"kpa:"COLOR_END);
    if (!GetCurrentDir(&curr_dir)) {
      return false;
    }
    printf(COLOR_BLUE"%s>> "COLOR_END, curr_dir);
    free(curr_dir);
    if (!ReadLine(&line)) {
      return false;
    }
    ParseResult token_res = TokenizeLine(line, &args, &num_args);
    if (token_res != PARSE_OK) {
      free(line);
      if (token_res == PARSE_SYSTEM_ERROR) {
        return false;
      }
      printf(ERROR"argument missing balanced quotes\n"COLOR_END);
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
      printf(ERROR"expected file name for i/o redirection\n"COLOR_END);
      continue;
    }
    if (DEBUG) {
      printf("num args: %d\n", cmd->num_args);
      for (int i = 0; i < cmd->num_args; i++) {
        printf("arg[%d]: %s\n", i, cmd->args[i]);
      }
      printf("input_file: %s\n", cmd->input_file);
      printf("output_file: %s\n", cmd->output_file);
      if (cmd->append == 0) {
        printf("append false\n");
      } else {
        printf("append true\n");
      }
    }
    status = Launch(cmd);
    free(line);
    free(args);
    free(cmd);
  } while (status);
  
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
        // This is what I would consider a critical error
        // like a malloc fail. Any error should be dev facing 
        // rather than user facing.
        perror("getcwd failed in GetCurrentDir");
        return false;
      }
    }
  } while (ptr == NULL);
  *path_ptr_ret = dirname;
  return true;
}
