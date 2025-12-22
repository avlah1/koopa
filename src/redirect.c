#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "colors.h"

// Global flags with options for open syscall
int open_flags = O_CREAT | O_WRONLY | O_TRUNC;

// Macro function for syscall error detection
#define SYS_ERR_CHECK(X) do { \
	int retval = (X); \
	if (retval == -1) { \
		fprintf(stderr,  ERROR "%s\n", strerror(errno)); \
		exit(EXIT_FAILURE); \
	} \
} while(0)

// Finds either > or >> and updates global accordingly. Splits the args buffer to separate redirection destination from the rest of the arguments. Returns the file destination if either symbol was found, returns NULL otherwise.
char** get_redirect_dest(char** args) {
	int i = 0;
	int redirect_flag = 0;

	while (args[i] != NULL) {
		if (strcmp(args[i], ">") == 0) { 
			redirect_flag = 1;
		} else if (strcmp(args[i], ">>") == 0) {
			redirect_flag = 1;
			open_flags = O_CREAT | O_WRONLY | O_APPEND;
		}
		
		if (redirect_flag) {
			args[i] = NULL;
			return &args[i+1];
		}

		i++;
	}
	return NULL;
}

// Redirection entry point. Calls redirect dest function, and if a file was not returned, do nothing.  Otherise, we open a file, redirect stdout to new file, and close the new file.
void find_redirection(char** args) {
	
	char** filename = get_redirect_dest(args);

	if (filename) {
		// ALL THE SYSCALLS
		int file_desc = open(filename[0], open_flags, 0644);

		SYS_ERR_CHECK(file_desc);

		SYS_ERR_CHECK(dup2(file_desc, 1));

		SYS_ERR_CHECK(close(file_desc));
	}


}

