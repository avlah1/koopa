#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "../include/Koopa.h"
#include "../include/InputHandler.h"
#include "../include/Colors.h"

#define BUFSIZE 256

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
  bool status;
  
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
    ParseResult parse_res = ParseLine(line, &args, &num_args);
    if (parse_res != PARSE_OK) {
      free(line);
      if (parse_res == PARSE_SYSTEM_ERROR) {
        return false;
      }
      printf(ERROR"argument missing balanced quotes\n"COLOR_END);
      continue;
    }
    free(line);
    free(args);
  } while (true);
  return true;
}

bool GetCurrentDir(char** path_ptr_ret) {
  size_t buffer_size = BUFSIZE;
  char* ptr = NULL;
  char* buffer;
  do {
    buffer = malloc(sizeof(char) * buffer_size);
    if (buffer == NULL) {
      perror("malloc failed in GetCurrentDir");
      return false;
    }
    ptr = getcwd(buffer, buffer_size);
    if (ptr == NULL) {
      free(buffer);
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

  *path_ptr_ret = buffer;
  return true;
}
