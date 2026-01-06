#ifndef EXECUTE_H
#define EXECUTE_H

struct command_info {
	char** redirect_out;
	char** redirect_in;
	char** file;
	char** piped;
	int pipe_position;
	
};


#endif
