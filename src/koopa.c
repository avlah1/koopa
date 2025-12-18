#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "input_handler.h"
#include "execute.h"

void shell_loop() {
	char* line;
	char** args;
        int status;
	
	do {
		printf("kpa>> ");
		
		line = read_line();
	        printf("Line: %s\n", line);

		if (strcmp(line, "exit") == 0) {
			break;
		}

		args = parse_line(line);
		printf("Args: %s %s %s\n", args[0], args[1], args[2]);
		status = execute(args);
		
		free(line);
		free(args);

	} while (status);
	
	
}


int main() {

	shell_loop();

	exit(EXIT_SUCCESS);
}
