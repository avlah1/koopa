#ifndef EXECUTE_H_
#define EXECUTE_H_
#include <stdbool.h>

#include "InputHandler.h"
#include "Colors.h"
#include "ShellTypes.h"

// Walks a CommandChain and executes each Command according to its cond_op,
// skipping commands whose conditions are not met based on last_exit_code. Updates last _exit_code after
// each command that runs. Returns SHELL_EXIT if the exit buiilt-in is encountered, SHELL_CONTINUE otherwise.
ShellStatus Execute(CommandChain* chain);

#endif  // EXECUTE_H_