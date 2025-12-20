#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define BUFSIZE 128

// Buffer size for token buffer
#define TOK_BUFSIZE 64

// Delimiters for parse_line function
#define TOK_DELIMS " \n\t" 


// Tokenizes given line using the delimiters macro. 
char** parse_line(char* line) {
	int buffer_size = TOK_BUFSIZE;
	int position = 0;
	char** tokens = malloc(buffer_size * sizeof(char*));
	char* token;

	// Parent process will exit failure on allocator fail	
	if (!tokens) {
		fprintf(stderr, "error: allocator fail while parse_line\n");
		exit(EXIT_FAILURE);
	}
	
	// Get token and add to args array
	token = strtok(line, TOK_DELIMS);
	while (token != NULL) {
		tokens[position] = token;
		position++;

		// Request more buffer space if needed
		if (position >= buffer_size) {
			buffer_size += TOK_BUFSIZE;
			tokens = realloc(tokens, buffer_size * sizeof(char*));
			if (!tokens) {
				fprintf(stderr, "error: allocator fail while parse_line\n");
				exit(EXIT_FAILURE);
			}
		}
                
		// Continue parsing starting from where we left off
		token = strtok(NULL, TOK_DELIMS);
	}
        
	// Set the final character to null and return the array
	tokens[position] = NULL;
	return tokens; 
}

// Reads characters from stdin, placing each character in the buffer. When new line character is read (ie the command is terminated by the user), place a null character and return buffer. 
char* read_line() {
	int buffer_size = BUFSIZE;
	char* buffer = malloc(sizeof(char) * buffer_size);
	int position = 0;
	int c;

	if (!buffer) {
		fprintf(stderr, "error: allocator fail during read line\n");
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
				fprintf(stderr, "error: allocator fail during read line\n");
				exit(EXIT_FAILURE);
			}
		}
	}

}

