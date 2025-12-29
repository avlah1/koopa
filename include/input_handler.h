#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

char** parse_line(char* line);
char* read_line();
char** get_redirect_dest(char** args);

#endif
