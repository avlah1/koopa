#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

// A file for built in shell functions that do not necessitate forking.

int kpa_exit(char**);
int kpa_cd(char**);

// String identifiers for inbound built in commands
char* builtins_strs[] = {
	"exit",
	"cd"
};

int num_builtins() {
	return sizeof(builtins_strs) / sizeof(char*);
}

// Function pointers that map to the identifiers in builtins_strs[i]
int (*built_ins[])(char**) = {
	&kpa_exit,
	&kpa_cd
};

// Built-in function for changing directories
int kpa_cd(char** args) {
	// Check for the directory to change to in args[1]. If null, print error.
	if (args[1] == NULL) {
		fprintf(stderr, "error: expected directory for cd\n");
	} else {
		// Call chdir with given file. If returns nonzero, print error
		if (chdir(args[1]) != 0) {
			fprintf(stderr, "error = %s\n", strerror(errno));
		}
	}

	// Return success whether successful or not
	return 1;
}

// Built-in function for exit command
int kpa_exit(char**) {
	return 0;
}

