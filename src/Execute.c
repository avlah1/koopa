#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>


#include "../include/Execute.h"
#include "../include/Colors.h"

//*****************Built-ins************************/
static bool KpaCd(char** args) {
  if (args[1] == NULL) {
    fprintf(stderr, ERROR  " expected directory for cd\n");
  } else {
    if (chdir(args[1]) != 0) {
      fprintf(stderr, ERROR " %s\n", strerror(errno));
    }
  }
  return true;
}

static bool KpaExit(char** args) {
  return false;
}

static char* BuiltInsStrs[] = {
	"exit",
	"cd"
};

static int NumBuiltIns() {
  return sizeof(BuiltInsStrs) / sizeof(char**);
}

static bool (*built_ins[])(char**) = {
  &KpaExit,
  &KpaCd
};
//************************************************/

bool Launch(char** args) {
  if (args[0] == NULL) {
    return false;
  }
  int n = NumBuiltIns();
  for (int i = 0; i < n; i++) {
    if (strcmp(args[0], BuiltInsStrs[i]) == 0) {
      return (*built_ins[i])(args);
    }
  }
  return true; // TODO: change this
}

