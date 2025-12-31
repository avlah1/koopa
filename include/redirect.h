#ifndef REDIRECT_H
#define REDIRECT_H

struct command_info {
	char** redirect_out;
	char** redirect_in;
	char** file;
	char** piped;
	int arg_position;
};

void parse_command(char** args, struct command_info* info);

void redirect_out(char** args, struct command_info* info);

void redirect_in(char** args);

#endif
