#ifndef COMMAND_CHAIN_H_
#define COMMAND_CHAIN_H_
#include <stdbool.h>

#include "ShellTypes.h"

// TODO: DOCS 
// Allocates and initializes an emty CommandChain on the heap. 
// The caller owns the returned chain and is responsible for freeing it via 
// CommandChain_Free. Returns false if allocation fails.
bool CommandChain_Allocate(CommandChain** chain_ret);

// Appends a Command to the tail of a CommandChain, updating the head and tail
// pointers accordingly. Returns false if the append fails.
bool CommandChain_Append(CommandChain* chain, Command* cmd);

// Frees a Command struct and all memory it owns: each strdup'd arg in cmd->args,
// the args array itself, and the input/output filename strings if present.
void CommandChain_Free(CommandChain* chain);

// Frees a CommandChain and all Commands it contains by walking the linked list
// and calling CommandChain_FreeCommand on each node, then freeing the chain itself.
void CommandChain_FreeCommand(Command* cmd);

// Debug utility that prints the full contents of a CommandChain to stdout,
// including each Command's args, redirection files, append flag, and cond_op.
void CommandChain_PrintChain(CommandChain* chain);

#endif  // COMMAND_CHAIN_H_