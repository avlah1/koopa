#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>


#include "../include/Execute.h"


#define COMMAND_NOT_FOUND_EXIT_CODE 127

// For future use when the shell supports conditional operators
static int last_exit_code = 0;

//*****************Built-ins************************/

// TODO : Rewrite function spec
// TODO : change error handling in then block. maybe use perror instead of the literal you have in there
static int KpaCd(char** args) {
  if (args[1] != NULL) {
    if (chdir(args[1]) != 0) {
      fprintf(stderr, COLOR_RED "%s: %s" COLOR_END "\n", args[1], strerror(errno));
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}

static int KpaStatus(char** args) {
  fprintf(stdout, "%d\n", last_exit_code);
  return EXIT_SUCCESS;
}

// An array of strings that map to the names of built-in kpa functions.
static char* BuiltInsStrs[] = {
	"cd",
  "status"
};

// Returns the number of built-in functions.
static int NumBuiltIns() {
  return sizeof(BuiltInsStrs) / sizeof(char**);
}

// An array of function pointers built-in functions.
static int (*built_ins[])(char**) = {
  &KpaCd,
  &KpaStatus
};
//************************************************/

// Static helper that redirects input, output, or both. 
// If the struct pointed to by "cmd" has non-null input_file or output_file
// fields, stdin and/or stdout are redirected accordingly. If failure occurs when
// attempting to redirect, the process is exited with failure.
// This function never assumes responsibility for "cmd".
static void Redirect(Command* cmd) {
  if (cmd->input_file != NULL) {
    int fd = open(cmd->input_file, O_RDONLY);
    if (fd == -1) {
      perror("open failed for input redirection");
      exit(EXIT_FAILURE);
    }
    dup2(fd, STDIN_FILENO);
    close(fd);
  }
  if (cmd->output_file != NULL) {
    int flags =  O_CREAT | O_WRONLY | (cmd->append ? O_APPEND : O_TRUNC);
    int fd = open(cmd->output_file, flags, 0644);
    if (fd == -1) {
      perror("open failed for input redirection");
      exit(EXIT_FAILURE);
    }
    dup2(fd, STDOUT_FILENO);
    close(fd);
  }
}

// Static helper that supports fork/exec logic for "standard" commands.
// Forks a child process. The child redirects i/o (if needed), then calls execvp
// to begin execution of a new program specificed by cmd->args[0]. An error message 
// prints to stderr if execvp fails, and depending on the value of errno, will exit
// with failure or with a special "command not found" macro. 
// If fork fails, -1 is returned.
// The parent process waits for the child process to either exit normally (whether via success or failure),
// or be terminated by an uncaught signal. If the child process exited normally, the status is returned to the caller,
// otherwise, -1 is returned.
// This function never assumes responsibility for "cmd".
static int StandardLaunch(Command* cmd) {
  pid_t pid;
  int status;
  pid = fork();
  if (pid == 0) {
    // Child process
    Redirect(cmd);
    if (execvp(cmd->args[0], cmd->args) == -1) {
      if (errno == ENOENT) {
        fprintf(stderr, ERROR" %s: command not found\n", cmd->args[0]);
        exit(COMMAND_NOT_FOUND_EXIT_CODE);
      } else {
        perror("execvp failed in LaunchStandard");
        exit(EXIT_FAILURE);
      }
    }
  } else if (pid < 0) {
    perror("fork failed in LaunchStandard");
    return -1;
  } else {
    // Parent process
    do {
      // We will add WNOHANG when we handle background logic
      waitpid(pid, &status, WUNTRACED | WCONTINUED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }
  // Did the program exit (whether successful or failed)
  if (WIFEXITED(status)) {
    // Extracts either 0 (success) or 1 (fail)
    return WEXITSTATUS(status);
  }
  return -1;
}

static int Launch(Command* cmd) {
  int n = NumBuiltIns();
  for (int i = 0; i < n; i++) {
    if (strcmp(cmd->args[0], BuiltInsStrs[i]) == 0) {
        return (*built_ins[i])(cmd->args);
    }
  }
  // TODO: add logic for PipelineLaunch
  return StandardLaunch(cmd);
}

ShellStatus Execute(CommandChain* chain) {
  Command* cmd = chain->head;
  while (cmd != NULL) {
    int exit_code;
    CondOpInfo op = cmd->cond_op;
    if (last_exit_code == 0) {
      // the previous command successful
      if (op == OP_AND || op == OP_SEP || op == OP_NONE) {
        if (strcmp(cmd->args[0], "exit") == 0) {
          return SHELL_EXIT;
        }
        exit_code = Launch(cmd);
        last_exit_code = exit_code;
      }
    } else {
      if (op == OP_OR || op == OP_SEP || op == OP_NONE) {
        if (strcmp(cmd->args[0], "exit") == 0) {
          return SHELL_EXIT;
        }
        exit_code = Launch(cmd);
        last_exit_code = exit_code;
      }
    }
    cmd = (Command*) cmd->next;
  }
  return SHELL_CONTINUE;
}



