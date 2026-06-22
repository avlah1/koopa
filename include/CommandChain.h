#ifndef COMMAND_CHAIN_H_
#define COMMAND_CHAIN_H_
#include <stdbool.h>

#include "ShellTypes.h"


// Allocator
bool CommandChain_Allocate(CommandChain** chain_ret);


// Appender
bool CommandChain_Append(CommandChain* chain, Command* cmd);

// Freer
void CommandChain_Free(CommandChain* chain);

// Individual Freer
void CommandChain_FreeCommand(Command* cmd);

void CommandChain_PrintChain(CommandChain* chain);

#endif  // COMMAND_CHAIN_H_