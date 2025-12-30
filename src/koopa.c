#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "input_handler.h"
#include "execute.h"
#include "colors.h"

#define BUFSIZE 128
#define DEBUG 0

// Helper to get current working directory and display to terminal. Originally, I wanted to put this function elsewhere but I couldn't quite find an appropriate src file to put it in. 
void get_current_directory() {
	
	char* buffer = NULL;
	char* ptr = NULL;
	size_t size = BUFSIZE;

	do {
		// Allocate memory for the incoming string
		buffer = realloc(buffer, size);
		
		// Check for faiilure to allocate, print error, then return.
		if (buffer == NULL) {
			fprintf(stderr, ERROR " allocator fail during gcd\n");
			return;
		}
		
		// Call getcwd with the allocated buffer and size
		ptr = getcwd(buffer, size);
		
		// Syscall could fail for: not enough space OR any other reason
		if (ptr == NULL) {
			if (errno == ERANGE) {
				size *= 2;
			} else {
				fprintf(stderr, ERROR "%s\n", strerror(errno));
			        return;	
			}
		}

	} while (ptr == NULL);
	
	printf(COLOR_BLUE"%s>> "COLOR_END, buffer);
	free(buffer);
}


// While status is nonzero, read line from stdin, separate line into args, then call execute to either fork or call builtin shell functions functions
void shell_loop() {
	
	char* line;
	char** args;
        int status;
	
	do {
		printf(COLOR_GREEN"kpa:"COLOR_END);
		get_current_directory();
		
		line = read_line();
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
