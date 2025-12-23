#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "input_handler.h"
#include "execute.h"
#include "colors.h"

// While status is nonzero, read line from stdin, separate line into args, then call execute to either fork or call builtin shell functions functions
void shell_loop() {
	char* line;
	char** args;
        int status;
	
	do {
		printf(COLOR_GREEN"kpa>> "COLOR_END);
		
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
