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

// Macro function for syscall error detection
#define SYS_ERR_CHECK(X) do { \
	int retval = (X); \
	if (retval == -1) { \
		fprintf(stderr, "syscall error = %s\n", strerror(errno)); \
		exit(EXIT_FAILURE); \
	} \
} while(0)
			
		

// Initializes child process specified by arg at index 0. Child process exits on failure.
int launch(char** args) {
	pid_t pid; 
	int status;
	
	pid = fork();

	if (pid == 0) {
		// CHILD PROCESS
		
		// Look for output redirection symbol	
		char** filename = get_redirect_dest(args);
		
		if (filename) {

			// Open file, redirect stdout, close fd returned by open, checking for syscall errors along the way.
			int file_desc = open(filename[0], O_CREAT | O_WRONLY | O_TRUNC, 0644);
			
			SYS_ERR_CHECK(file_desc);

			SYS_ERR_CHECK(dup2(file_desc, 1));

			SYS_ERR_CHECK(close(file_desc));
		}
		
		// Opted not to use the macro here as this syscall operates a bit differently than the others that are in the previous conditional block. That is, if execv returns at all, we should exit. 
		if (execvp(args[0], args) == -1) {
			fprintf(stderr, "execvp error = %s\n", strerror(errno));
		}
		
		exit(EXIT_FAILURE);

	} else if (pid < 0) {
		perror("koopa");

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

