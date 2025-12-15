#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "input_handler.h"
#include "execute.h"
#include "built_ins.h"

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
