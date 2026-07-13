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

// Changes the current working directory to args[1]. Returns EXIT_FAILURE
// and prints an error if chdir fails, or EXIT_SUCCESS if no directory
// was provided or the change succeeded.
static int KpaCd(char** args) {
  if (args[1] != NULL) {
    if (chdir(args[1]) != 0) {
      perror("kpa: failed to change directory");
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}

// Prints the exit code of the last executed command to stdout, similar
// to bash's $?. Always returns EXIT_SUCCESS.
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

// Forks a child process and execs the command specified by cmd->args[0],
// with I/O redirection applied before exec. The parent waits for the child
// to terminate and returns its exit code via WEXITSTATUS. Returns -1 if
// fork fails or the child was killed by a signal rather than exiting normally.
static int StandardExecute(Command* cmd) {
  pid_t pid;
  int status;
  pid = fork();
  if (pid == 0) {
    // Child process
    Redirect(cmd);
    if (execvp(cmd->args[0], cmd->args) == -1) {
      if (errno == ENOENT) {
        fprintf(stderr, "kpa: %s: command not found\n", cmd->args[0]);
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

// Executes a pipeline of num_cmds commands connected by Unix pipes. Forks one
// child per command, wiring stdout -> pipe write end for all but the last command,
// and stdin -> pipe read end for all but the first command. Each child applies I/O
// redirection via Redirect() before exec. The parent closes pipe ends
// as long as they are no longer needed after each fork. All children are waited
// for after the fork loop. Returns the exit code of the last command in the pipeline
// or -1 if fork fails or the last child was killed by a signal.
static int PipedExecute(Command* cmd, int num_cmds) {
  int status;
  bool first, last;
  pid_t pids[num_cmds];
  Command* curr = cmd;
  int prev_read = -1;
  for (int i = 0; i < num_cmds; i++) {
    int pipefd[2];
    first = (i == 0);
    last = (i == num_cmds - 1);
    // Only create a new pipe if this isn't the last piped command
    if (!last) {
      if (pipe(pipefd) == -1) {
        perror("pipe failed in PipedExecute");
        return EXIT_FAILURE;
      }
    }
    pid_t pid = fork();
    pids[i] = pid;
    if (pid == 0) {
      // Child process
      Redirect(curr);
        // Only redirect stdin if not the first piped command
        if (!first) {
          dup2(prev_read, STDIN_FILENO);
          close(prev_read);
        }
        // Only redirect output to pipe if not the last piped command
        if (!last) {
          dup2(pipefd[1], STDOUT_FILENO);
          close(pipefd[0]);
          close(pipefd[1]);
        }
        if (execvp(curr->args[0], curr->args) == -1) {
            if (errno == ENOENT) {
              fprintf(stderr, "kpa: %s: command not found\n", curr->args[0]);
              exit(COMMAND_NOT_FOUND_EXIT_CODE);
            } else {
              perror("execvp failed in LaunchStandard");
              exit(EXIT_FAILURE);
            }
        }
    } else if (pid < 0) {
      perror("fork failed in PipedExecute");
      // Wait for the children that have been spawned so far, if any.
      for (int j = 0; j < i; j++) {
        waitpid(pids[j], NULL, 0);
      }
       return -1;
    } else {
      // Parent process
      if (!first) {
        close(prev_read);
      }
      // Save the read end of the pipe so it can be used in the next child process.
      if (!last) {
        prev_read = pipefd[0];
        close(pipefd[1]);
      }
      curr = (Command*) curr->next;
    }
  }
  // Wait for all children we have spawned
  for (int i = 0; i < num_cmds; i++) waitpid(pids[i], &status, 0);
  if (WIFEXITED(status)) {
    return WEXITSTATUS(status);
  }
  return -1;
}

// Counts the number of commands in a pipeline starting at cmd by walking the chain
// until pipe_next is false, then calls PipedExecute. Sets cmd_ret to the last command
// in the pipeline so the caller knows where to resume walking the CommandChain after
// pipeline completes.
// Returns exit code of pipeline from PipedExecute.
static int PipedLaunch(Command* cmd, Command** cmd_ret) {
  Command* curr = cmd;
  int num_piped = 1;
  while (curr->pipe_next) {
    curr = (Command*) curr->next;
    num_piped++;
  }
  *cmd_ret = curr;
  return PipedExecute(cmd, num_piped);
}

// Routes a Command to the appropriate built-in handler or StandardLaunch.
// Returns the exit code of whichever handler runs.
static int StandardLaunch(Command* cmd) {
  int n = NumBuiltIns();
  for (int i = 0; i < n; i++) {
    if (strcmp(cmd->args[0], BuiltInsStrs[i]) == 0) {
        return (*built_ins[i])(cmd->args);
    }
  }
  return StandardExecute(cmd);
}


ShellStatus Launch(CommandChain* chain) {
  Command* last_piped_cmd;
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
        if (cmd->pipe_next) {
          exit_code = PipedLaunch(cmd, &last_piped_cmd);
          cmd = last_piped_cmd;
        } else {
          exit_code = StandardLaunch(cmd);
        }
        last_exit_code = (exit_code == -1) ? 1 : exit_code;
      } else {
        while (cmd->pipe_next) {
            cmd = (Command*) cmd->next;
        }
      }
    } else {
      // previous command was unsuccessful
      if (op == OP_OR || op == OP_SEP || op == OP_NONE) {
        if (strcmp(cmd->args[0], "exit") == 0) {
          return SHELL_EXIT;
        }
        if (cmd->pipe_next) {
          exit_code = PipedLaunch(cmd, &last_piped_cmd);
          cmd = last_piped_cmd;
        } else {
          exit_code = StandardLaunch(cmd);
        }
        last_exit_code = (exit_code == -1) ? 1 : exit_code;
      } else {
        while (cmd->pipe_next) {
            cmd = (Command*) cmd->next;
        }
      }
    }
    cmd = (Command*) cmd->next;
  }
  return SHELL_CONTINUE;
}



