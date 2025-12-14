#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// Initializes process specified by the first arg, returns 1 when process terminates.
int execute(char** args) {
	pid_t pid; 
	int status;
	
	// Create child, store pid
	pid = fork();

	if (pid == 0) {
		// Child process

		// If exec returns AT ALL, exit failure. Note that if exec is successful, it returns nothing.
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

		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}

	return 1;
}
void shell_loop() {
	char* line;
	char** args;
        int status;
	

	do {
		printf("kpa>> ");
		line = read_line();
		
		if (strcmp(line, "exit") == 0) {
			break;
		}

		args = parse_line(line);

		status = execute(args);
		free(line);
		free(args);

	} while (status);
	
	
}


int main() {

	shell_loop();

	exit(EXIT_SUCCESS);
}
