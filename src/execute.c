#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h> 
#include <errno.h>

#include "builtins.h"
#include "input_handler.h"
#include "redirect.h"
#include "colors.h"

// Initializes child process specified by arg at index 0. Child process exits on failure.
int launch(char** args) {
	
	pid_t pid; 
	int status;
	
	pid = fork();

	if (pid == 0) {
		// CHILD PROCESS
	       	// Check to see if a redirection symbol is present in the args list, if it is, do stuff (see redirect.c). Otherwise, don't do stuff. In either case, call execvp. 
		find_redirection(args);	

		// Opted not to use the macro here as this syscall operates a bit differently than the others that are in the previous conditional block. That is, if execv returns at all, we should exit. 
		if (execvp(args[0], args) == -1) {
			fprintf(stderr, ERROR "%s\n", strerror(errno));
		}
		
		exit(EXIT_FAILURE);
	
	// Failure to fork
	} else if (pid < 0) {
		fprintf(stderr, ERROR"%s\n", strerror(errno));
	} else {
		// PARENT PROCESS
		do {
			// Wait for child process to terminate or stop. If terminated, break, otherwise, continue.
			waitpid(pid, &status, WUNTRACED);
		
		// As soon as the child process has exited normally, or the child process has been killed by a signal, then the loop breaks
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}

	return 1;
}

// An entry point to detect whether the given arg in index 0 is built in, or if forking is required.
int execute(char** args) {
	
	if (args[0] == NULL) {
		return 1;      
	}
	
	int i;
	int n = num_builtins();

	for (i = 0; i < n; i++) {
		if (strcmp(args[0], builtins_strs[i]) == 0) {
			return (*built_ins[i])(args);
		}
	}	
	
	return launch(args);
}

