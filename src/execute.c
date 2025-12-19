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

#define SYS_ERR_CHECK(X) do { \
	int retval = (X); \
	if (retval == -1) { \
		fprintf(stderr, "syscall error = %s\n", strerror(errno)); \
		exit(EXIT_FAILURE); \
	} \
} while(0)
			
		

// Initializes process specified by the first arg, returns 1 when process terminates.
int launch(char** args) {
	pid_t pid; 
	int status;
	
	// Create child, store pid
	pid = fork();

	if (pid == 0) {
		// Child process
		//examine args. look for > and split
		
		char** filename = get_redirect_dest(args);
		SYS_ERR_CHECK(open("/this/file/does/not/exist", O_RDONLY));
		if (filename) {
			int file_desc = open(filename[0], O_CREAT | O_WRONLY | O_TRUNC, 0644);
			
			SYS_ERR_CHECK(file_desc);

			SYS_ERR_CHECK(dup2(file_desc, 1));

			SYS_ERR_CHECK(close(file_desc));

		}
			
		/*
		if (filename) {
			int file_desc = open(filename[0], O_CREAT | O_WRONLY | O_TRUNC, 0644);

			if (file_desc == -1) {
				perror("Error opening file");
				//exit(EXIT_FAILURE);
			}

			//dup2
			if (dup2(file_desc, 1) == -1) {
			       perror("Error duplicating descriptor");
			}
	 		
			// close?
			if (close(file_desc) == -1) {
				perror("Error closing file dsc");
			}
					
		}
		*/
		
		if (execvp(args[0], args) == -1) {
			perror("execvp");
		}
		
		exit(EXIT_FAILURE);

	} else if (pid < 0) {
		
		// Fork failure if it returns negative, so update errno and show error
		perror("koopa");

	} else {
		// Parent process

		do {
			// Wait for child process to terminate or stop. If terminated, break, otherwise, continue.
			waitpid(pid, &status, WUNTRACED);
		
		// As soon as the child process has exited normally, or the child process has been killed by a signal, then the loop breaks
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}

	return 1;
}

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

