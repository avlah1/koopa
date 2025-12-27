#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "input_handler.h"
#include "execute.h"
#include "colors.h"

#define BUFSIZE 128

void get_current_directory() {

	char* buffer = NULL;
	char* ptr = NULL;
	size_t size = BUFSIZE;

	do {
		buffer = realloc(buffer, size);

		if (buffer == NULL) {
			fprintf(stderr, ERROR " allocator fail during gcd\n");
			// I dont think we want a crash at this point, so maybe just return
			return;
		}

		ptr = getcwd(buffer, size);

		if (ptr == NULL) {
			if (errno == ERANGE) {
				size *= 2;
			} else {
				fprintf(stderr, ERROR "%s\n", strerror(errno));
			       return;	
			}
		}
	} while (ptr == NULL);
	
	printf(COLOR_BLUE"%s"COLOR_END, buffer);
	free(buffer);


		
}


// While status is nonzero, read line from stdin, separate line into args, then call execute to either fork or call builtin shell functions functions
void shell_loop() {
	char* line;
	char** args;
        int status;
	
	do {
		printf(COLOR_GREEN"kpa "COLOR_END);
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
