#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// Initializes process specified by the first arg, returns 1 when process terminates.
int execute(char** args) {
	pid_t pid; 
	int status;
	
	// Create child, store pid
	pid = fork();

	if (pid == 0) {
		// Child process

		// Check for no commands given, should exit success rather than seg fault
		if (args[0] == NULL) {
			exit(EXIT_SUCCESS);
		}

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
