#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h> 
#include <errno.h>

#include "builtins.h"
#include "execute.h"
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
	
	info->redirect_out[0] = NULL;
}


// Opens file to read from, redirect stdin to file. Clears the redirect token by setting position to NULL.
void redirect_in(char** args, struct command_info* info) {
	int file_desc = open(info->file[0], O_RDONLY);

	SYS_ERR_CHECK(file_desc);

	SYS_ERR_CHECK(dup2(file_desc, STDIN_FILENO));

	SYS_ERR_CHECK(close(file_desc));

	info->redirect_in[0]= NULL;
}

void parse_command(char** args, struct command_info* info) {
	
	int i = 0;
	int redirect_flag = 0;

	while (args[i] != NULL) {
		if ((strcmp(args[i], ">") == 0) || (strcmp(args[i], ">>") == 0)) {
			info->redirect_out = &args[i];
			redirect_flag = 1;
			break;
		} else if (strcmp(args[i], "<") == 0) {
			info->redirect_in = &args[i];
			redirect_flag = 1;
			break;
		} else if (strcmp(args[i], "|") == 0) {
			info->piped = &args[i];
			info->pipe_position = i;
		}
		i++;
	}

	if (!redirect_flag) return;

	info->file = &args[i + 1];
}

int launch_pipeline(char** args, struct command_info* info) {
	int pipefd[2];

	pid_t pid1;
	pid_t pid2;

	SYS_ERR_CHECK(pipe(pipefd));
	
	info->piped[0] = NULL;

	pid1 = fork();

	if (pid1 == 0) {
		// dup2, close, exec
		SYS_ERR_CHECK(dup2(pipefd[1], STDOUT_FILENO));
		SYS_ERR_CHECK(close(pipefd[0]));
		SYS_ERR_CHECK(close(pipefd[1]));
		
		if (execvp(args[0], args) == - 1) {
			fprintf(stderr, ERROR "%s\n", strerror(errno));
		}

	} else if (pid1 == -1) {
		fprintf(stderr, ERROR"%s\n", strerror(errno));
	}	

	pid2 = fork();
	if (pid2 == 0) {
		// dup2 close exec
		SYS_ERR_CHECK(dup2(pipefd[0], STDIN_FILENO));
		SYS_ERR_CHECK(close(pipefd[0]));
		SYS_ERR_CHECK(close(pipefd[1]));
		
		if (execvp(args[info->pipe_position + 1], &args[info->pipe_position + 1]) == -1) {
			fprintf(stderr, ERROR "%s\n", strerror(errno));
	
		} 
	} else if (pid2 == - 1) {
		fprintf(stderr, ERROR"%s\n", strerror(errno));
	}

	close(pipefd[0]);
	close(pipefd[1]);


	wait(NULL);
	wait(NULL);
	
	return 1;
}


// Initializes child process specified by arg at index 0. Child process exits on failure.
int launch_standard(char** args, struct command_info* info) {
	
	pid_t pid; 
	int status;
		
	pid = fork();

	if (pid == 0) {
		// CHILD PROCESS
	
		// Check which fields in the struct have been updated and call the corresponding redirection function	
		if (info->redirect_out) {
			redirect_out(args, info);
		} else if (info->redirect_in) {
			redirect_in(args, info);
		}	
		
		if (execvp(args[0], args) == -1) {
			fprintf(stderr, ERROR "%s\n", strerror(errno));
		}
		
		exit(EXIT_FAILURE);
	
	// Failure to fork
	} else if (pid < 0) {
		fprintf(stderr, ERROR"%s\n", strerror(errno));
	} else {
		// PARENT PROCESS
		do {
			// Wait for child process to terminate or stop. If terminated, break, otherwise, continue.
			waitpid(pid, &status, WUNTRACED);
		
		// As soon as the child process has exited normally, or the child process has been killed by a signal, then the loop breaks
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}

	return 1;
	
}

// An entry point to detect whether the given arg in index 0 is built in, or if forking is required.
int execute(char** args) {
	
	if (args[0] == NULL) {
		return 1;      
	}
	
	int i;
	int n = num_builtins();

	for (i = 0; i < n; i++) {
		if (strcmp(args[0], builtins_strs[i]) == 0) {
			return (*built_ins[i])(args);
		}
	}	
	
	struct command_info info = {0};
	parse_command(args, &info);
	
	if (info.piped) {
		return launch_pipeline(args, &info);
	} 
	
	return launch_standard(args, &info);
}

