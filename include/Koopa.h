#ifndef KOOPA_H_
#define KOOPA_H_

#include <stdbool.h>

#include "InputHandler.h"
#include "Execute.h"
#include "Colors.h"
#include "ShellTypes.h"

// Function spec incoming. This function keeps changing as more functionality 
// is added. I am holding off on writing a description for it for now.
bool ShellLoop();

// Gets the absolute file path of the current working directory, and stores
// it as a newly heap-allocated C-string. A pointer to the string is placed 
// in the return parameter "path_ret". The caller assumes responsibility of 
// this heap-allocated memory if and only if the function returns true.
// Arguments:
// - path_ret: the return parameter where the C-string path name is placed. 
//   Only valid if the function returns true; caller owns this memory and is
//   responsible for freeing it.
// Returns:
// - true if successful
// - false if a system failure occurs while getting/allocating the C-string
//   path name (eg, malloc fail).
bool GetCurrentDir(char** path_ret);

#endif  // KOOPA_H_