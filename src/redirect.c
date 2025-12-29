#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "colors.h"

// Macro function for syscall error detection
#define SYS_ERR_CHECK(X) do { \
	int retval = (X); \
	if (retval == -1) { \
		fprintf(stderr,  ERROR "%s\n", strerror(errno)); \
		exit(EXIT_FAILURE); \
	} \
} while(0)



// Determines if clobbering or appending and updates flags accordingly. Opens file, redirect stdout to the file. Clears the redirect token by setting position to NULL. 
void redirect_out(char** args) {
	
	int open_flags = O_CREAT | O_WRONLY;

	if (strcmp(args[0], ">>") == 0) {
		open_flags |= O_APPEND;
	} else {
		open_flags |= O_TRUNC; 
	}

	int file_desc = open(args[1], open_flags, 0644);

	SYS_ERR_CHECK(file_desc);

	SYS_ERR_CHECK(dup2(file_desc, 1));

	SYS_ERR_CHECK(close(file_desc));

	args[0] = NULL;
}

// Opens file to read from, redirect stdin to file. Clears the redirect token by setting position to NULL.
void redirect_in(char** args) {
	
	int file_desc = open(args[1], O_RDONLY);

	SYS_ERR_CHECK(file_desc);

	SYS_ERR_CHECK(dup2(file_desc, STDIN_FILENO));

	SYS_ERR_CHECK(close(file_desc));

	args[0] = NULL;
}

// Redirection entrypoint. Loops through args, if a redirection token is found, calls the appropriate helper. Otherwise, return to caller.
void find_redirection(char** args) {
	
	int i = 0;

	while (args[i] != NULL) {
		if ((strcmp(args[i], ">") == 0) || strcmp(args[i], ">>") == 0) {
			redirect_out(&args[i]);	
		} else if (strcmp(args[i], "<") == 0) {
			redirect_in(&args[i]);
		}
		i++;
	}	

	return;
}

