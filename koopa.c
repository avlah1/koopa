#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// ***MACROS***

// Buffer size for line buffer
#define BUFSIZE 128

// Buffer size for token buffer
#define TOK_BUFSIZE 64

// Delimiters for parse_line function
#define TOK_DELIMS " \n\t" 





// Reads characters from stdin, placing each character in the dynamically allocated buffer. When new line character is read (ie the command is terminated by the user), place a null character and return buffer. 
char* read_line() {
	int buffer_size = BUFSIZE;
	char* buffer = malloc(sizeof(char) * buffer_size);
	int position = 0;
	int c;

	if (!buffer) {
		printf("koopa line reader allocation error");
		exit(EXIT_FAILURE);
	}
	
	
	while (1) {
		c = getchar();
	        
		// Detect newline character, setting character to null terminator, otherwise set to current character
		if (c == '\n') {
			buffer[position] = '\0';
			return buffer;
		} else {
			buffer[position] = c;
		}
		position++;
                
		// Increase buffer size if needed
		if (position >= buffer_size) {
			buffer_size += BUFSIZE;
			buffer = realloc(buffer, buffer_size);
			if (!buffer) {
				printf("koopa line reader reallocation error");
				exit(EXIT_FAILURE);
			}
		}
	}


}

// Tokenizes given line using the delimiters macro.
char** parse_line(char* line) {
	int buffer_size = TOK_BUFSIZE;
	int position = 0;
	char** tokens = malloc(buffer_size * sizeof(char*));
	char* token;
	
	printf("address of tokens: %p\n", tokens);
	if (tokens == NULL) {
		printf("koopa parse line error");
		exit(EXIT_FAILURE);
	}
	
	token = strtok(line, TOK_DELIMS);
	while (token != NULL) {
		tokens[position] = token;
		printf("tokens[%d]: %s\n", position, tokens[position]);
		position++;

		// Request more buffer space if needed
		if (position >= buffer_size) {
			buffer_size += TOK_BUFSIZE;
			tokens = realloc(tokens, buffer_size * sizeof(char*));
			if (!tokens) {
				printf("koopa parse line reallocate error\n");
				exit(EXIT_FAILURE);
			}
		}
                
		// Continue parsing starting from where we left off
		token = strtok(NULL, TOK_DELIMS);
	}
        
	// Set the final character to null
	tokens[position] = NULL;
	printf("first arg: %s\n", tokens[0]);
	return tokens; 


}

int execute(char** args) {
	pid_t pid, wpid; 
	int status;

	pid = fork();

	if (pid == 0) {
		// child process
		printf(args[0]);
		if (execvp(args[0], args) == -1) {
			perror("execvp");
		}
		
		exit(EXIT_FAILURE);
	} else if (pid < 0) {
		printf("koopa fork error\n");	
	} else {
		// parent process logic
		do {
			wpid = waitpid(pid, &status, WUNTRACED);
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
	
	exit(EXIT_SUCCESS);
	
}


int main() {

	shell_loop();
	return 0;

}
