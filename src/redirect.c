#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include "colors.h"
#include "redirect.h"

// Macro function for syscall error detection
#define SYS_ERR_CHECK(X) do { \
	int retval = (X); \
	if (retval == -1) { \
		fprintf(stderr,  ERROR "%s\n", strerror(errno)); \
		exit(EXIT_FAILURE); \
	} \
} while(0)

// Determines if clobbering or appending and updates flags accordingly. Opens file, redirect stdout to the file. Clears the redirect token by setting position to NULL. 
void redirect_out(char** args, struct command_info* info) {
	
	int open_flags = O_CREAT | O_WRONLY;
	
	if (strcmp(info->redirect_out[0], ">>") == 0) {
		open_flags |= O_APPEND;
	} else {
		open_flags |= O_TRUNC; 
	}
	int file_desc = open(info->file[0], open_flags, 0644);

	SYS_ERR_CHECK(file_desc);

	SYS_ERR_CHECK(dup2(file_desc, 1));

	SYS_ERR_CHECK(close(file_desc));
	
	args[info->arg_position] = NULL;
}

// Opens file to read from, redirect stdin to file. Clears the redirect token by setting position to NULL.
void redirect_in(char** args) {
	
	int file_desc = open(args[1], O_RDONLY);

	SYS_ERR_CHECK(file_desc);

	SYS_ERR_CHECK(dup2(file_desc, STDIN_FILENO));

	SYS_ERR_CHECK(close(file_desc));

	args[0] = NULL;
}

void parse_command(char** args, struct command_info* info) {
	
	int i = 0;
	int redirect_flag = 0;

	while (args[i] != NULL) {
		if ((strcmp(args[i], ">") == 0) || (strcmp(args[i], ">>") == 0)) {
			printf("detected >\n");
			info->redirect_out = &args[i];
			redirect_flag = 1;
			break;
		} else if (strcmp(args[i], "<") == 0) {
			info->redirect_in = &args[i];
			break;
		} else if (strcmp(args[i], "|") == 0) {
			info->piped = &args[i];
		}
		i++;
	}
	if (redirect_flag) {
		info->file = &args[i + 1];
		info->arg_position = i;
	}
}

