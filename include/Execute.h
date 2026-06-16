#ifndef EXECUTE_H_
#define EXECUTE_H_

#include <stdbool.h>

// Based on a tokenized list of args, either spawns child processe(s) OR launches a
// built-in function that does not require forking.
// This function should not be called if the first argument in "args" is NULL.
// This function handles piping and i/o redirection accordingly.
// This function does not ever gain ownership of "args". Returns true 
// if the child processe(s) exited successfully, otherwise returns false.
bool Launch(char** args);

#endif  // EXECUTE_H_