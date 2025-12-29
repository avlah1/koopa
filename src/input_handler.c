#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "colors.h"

#define BUFSIZE 128

// Buffer size for token buffer
#define TOK_BUFSIZE 64

// Size for individual tokens
#define TOKEN_SIZE 32

// Delimiters for parse_line function
#define TOK_DELIMS " \n\t" 

int global_quoted = 1;

char* get_token(char* str, char* delimeters) {
	
	// Static storage for subsequent calls to get_token
	static char* next_token_start = NULL;
	
	size_t token_size = TOKEN_SIZE;

	// The token buffer. Characters from str will be transferred here.
	char* token_start = malloc(token_size * sizeof(char));

	if (!token_start) {
		fprintf(stderr, ERROR"allocator fail while get_token\n");
		exit(EXIT_FAILURE);
	}
	
	// Now, handle the input string. Find where we are in the string.
	if (str != NULL) {
		// First call to get_token
		next_token_start = str;
	} else {
		// Subsequent calls to get_token
		str = next_token_start;
	}
	
	// If we are at the end of the string, there is nothing left to parse. Return null.
	if (str == NULL || *str == '\0') {
		return NULL;
	}
	
	// Eat leading delimiters
	while (*str != '\0' && strchr(delimeters, *str) != NULL) {
		str++;
	}
	
	// Return null if we nothing but delimters are left
	if (*str == '\0') {
		next_token_start = NULL;
		return NULL;
	}
	
	// Flag to keep track of whether or not we are in the middle quotes
	int quoted = 0;

	// Set the token pointer to the beginning of the buffer
	int position = 0;
	
	while(*str != '\0') {
		// If quotes, toggle quoted flag and skip this character
		if (*str == '"') {
			quoted = !quoted;
			str++;
		// If we aren't in the middle of quotes and we are currently looking at a delimiter, break.
		} else if (!quoted && strchr(delimeters, *str) != NULL) {
			break;
		// Otherwise, move the current character into the buffer, increasing both the current buffer position and pointer to str
		} else {
			token_start[position] = *str;
			position++;
			str++;
		}
	}

	if (quoted) {
		fprintf(stderr, ERROR "argument missing balanced quotes\n");
		global_quoted = !global_quoted;
		return NULL;
	}

	if (*str != '\0') {
		*str = '\0';
		next_token_start = str + 1;
	} else {
		next_token_start = NULL;
	}
	
	return token_start;

}
// Tokenizes given line using the delimiters macro. 
char** parse_line(char* line) {
	int buffer_size = TOK_BUFSIZE;
	int position = 0;
	char** tokens = malloc(buffer_size * sizeof(char*));
	char* token;

	// Parent process will exit failure on allocator fail	
	if (!tokens) {
		fprintf(stderr, ERROR"error: allocator fail while parse_line\n");
		exit(EXIT_FAILURE);
	}
	
	// Get token and add to args array
	token = get_token(line, TOK_DELIMS);
	while (token != NULL) {
		tokens[position] = token;
		position++;

		// Request more buffer space if needed
		if (position >= buffer_size) {
			buffer_size += TOK_BUFSIZE;
			tokens = realloc(tokens, buffer_size * sizeof(char*));
			if (!tokens) {
				fprintf(stderr, ERROR"allocator fail while parse_line\n");
				exit(EXIT_FAILURE);
			}
		}
		// Continue parsing starting from where we left off
		token = get_token(NULL, TOK_DELIMS);
	}
        
	if (!global_quoted) {
		global_quoted = !global_quoted;
		position = 0;
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
		fprintf(stderr, ERROR"allocator fail during read line\n");
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
				fprintf(stderr, ERROR"allocator fail during read line\n");
				exit(EXIT_FAILURE);
			}
		}
	}

}

