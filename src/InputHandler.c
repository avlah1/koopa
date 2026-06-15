#include <stdlib.h>
#include <stdio.h>

#include "include/InputHandler.h"

#define BUFSIZE 256

bool ReadLine(char** line_ptr_ret) {
  int buffer_size = BUFSIZE;
  char* buffer = malloc(sizeof(char) * buffer_size);
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